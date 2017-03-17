#include "include/first.hpp"
#include "TCPSocket.hpp"
#include "MessageBuffer.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace SocketServer
{

static const unsigned int MaxNumberBlocksPerReadWrite = 10;
static const unsigned int MaxInMsgs = 100;

TCPSocket::TCPSocket() 
  : SocketBase(SOCK_STREAM) 
{
  int flag = ::fcntl(fd_, F_GETFL, 0);
  flag |= O_NONBLOCK;
  if(::fcntl(fd_, F_SETFL, flag) != 0) {
    logger << "socket=" << socketID_ << " failed to set NONBLOCK" << endlog;
  }
}

TCPSocket::TCPSocket(int fd, const std::string& peerIPAddress, unsigned int peerPort)
  : SocketBase(fd, peerIPAddress, peerPort) 
{
  int flag = ::fcntl(fd_, F_GETFL, 0);
  flag |= O_NONBLOCK;
  if(::fcntl(fd_, F_SETFL, flag) != 0) {
    logger << "socket=" << socketID_ << " failed to set NONBLOCK" << endlog;
  }
}

void 
TCPSocket::close()
{
  //MessageBuffer::Singleton().removeSocketMessages(getClientID());
  SocketBase::close();
}

int
TCPSocket::handleSelectReadable()
{
  logger << logger.debug << "socket=" << socketID_ << " select readable" << endlog;

  if (!isConnected_) {
    return checkNonBlockConnect_();
  }

  if (auto i = MessageBuffer::Singleton().getNumBufferedMessages(socketID_)) {
    if (i >= MaxInMsgs) {
      logger << "socket=" << socketID_ << " too many messages " << i << ":" << MaxInMsgs << ". stop read." << endlog;
      return 0;
    }
  }

  unsigned int totalNumBytesRead = 0;
  while (totalNumBytesRead < MaxNumberBlocksPerReadWrite * BlockBuffer::getSizePerBlock()) {
    int numBytesRead = ::read(fd_, recvBuffer_.getSpacePtr(), recvBuffer_.getContinuousSpaceSize());
    logger << logger.debug << "read bytes " << numBytesRead << endlog;
    if (numBytesRead == 0) {
      if (errno != EWOULDBLOCK) {
        logger << logger.test << "socket=" << socketID_ << " read EOF" << endlog;
        close();
      }
      break;
    }
    else if (numBytesRead < 0) {
      if (errno != EWOULDBLOCK) {
        logger << "socket=" << socketID_ << " read error " << strerror(errno) << endlog;
        close();
      }
      break;
    }
    recvBuffer_.resizePush(numBytesRead);
    totalNumBytesRead += numBytesRead;
  }

  // parse message and put into buffer
  while (MessageBuffer::Singleton().extractMessageFromSocket(recvBuffer_, socketID_)) {
  }

  return totalNumBytesRead;
}

int
TCPSocket::handleSelectWritable()
{
  logger << logger.debug << "socket=" << socketID_ << " select writable" << endlog;

  if (!isConnected_) {
    return checkNonBlockConnect_();
  }

  unsigned int totalBytesSend = 0;
  while (   sendBuffer_.getTotalDataSize() > 0 
         || MessageBuffer::Singleton().hasMessageToSend(socketID_)) {
    if (sendBuffer_.getTotalDataSize() == 0) {
      shared_ptr<MessageBase> pMsg = MessageBuffer::Singleton().popMessageToSend(socketID_);
      if (MessageBase::toBytes(sendBuffer_, *pMsg) <= 0) {
        logger << "socket=" << socketID_ << " failed to write message to buffer. " << pMsg->getName() << " ignored." << endlog; 
        return 0;
      }
    }

    if (   sendBuffer_.getContinuousDataSize() > 0 
        && totalBytesSend < MaxNumberBlocksPerReadWrite * BlockBuffer::getSizePerBlock()) {
      int numBytesSend = ::write(fd_, sendBuffer_.getDataPtr(), sendBuffer_.getContinuousDataSize());
      logger << logger.debug << "socket=" << socketID_ << " send " << numBytesSend << " bytes" << endlog;
      if (numBytesSend < 0) {
        if (errno != EAGAIN) {
          logger << "socket=" << socketID_ << " write error " << strerror(errno) << endlog;
          close();
        }
        break;
      }
      sendBuffer_.resizePop(numBytesSend);
      totalBytesSend += numBytesSend;
    }
    else {
      break;
    }
  }

  return totalBytesSend;
}

bool
TCPSocket::connectTo(const string& serverIPAddress, unsigned int serverPort)
{
  sockaddr_in peerSockAddr;
  socklen_t peerSockAddrLength = sizeof(peerSockAddr);
  peerSockAddr.sin_family= AF_INET;
  peerSockAddr.sin_port = htons(serverPort);
  inet_pton(AF_INET, serverIPAddress.c_str(), &peerSockAddr.sin_addr.s_addr);

  logger << "socket=" << socketID_ << " connecting to " << serverIPAddress << ":" << serverPort << endlog;
  if (::connect(fd_, (const sockaddr*)&peerSockAddr, peerSockAddrLength) == -1) {
    if (errno == EINPROGRESS) {
      return true;
    }
    else {
      logger << "socket=" << socketID_ << " connect failed: " << strerror(errno) << endlog;
      return false;
    }
  }
  
  return true;
}

bool
TCPSocket::hasBytesToSend() const
{
  if (   sendBuffer_.getTotalDataSize() > 0 
      || MessageBuffer::Singleton().hasMessageToSend(socketID_)) {
    return true;
  }

  return false;
}

int
TCPSocket::checkNonBlockConnect_()
{
  int err;
  socklen_t len = sizeof(err);
  if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len) == -1) {
    logger << "socket=" << socketID_ << " getsockopt failed " << strerror(errno) << endlog;
    close();
    return -1;
  }

  if (err) {
    logger << "socket=" << socketID_ << " connect error " << strerror(err) << endlog;
    close();
    return -1;
  }

  isConnected_ = true;

  // set local/peer address
  sockaddr_in localSockAddr;
  socklen_t localSockAddrLength = sizeof(localSockAddr);
  if (::getsockname(fd_, (sockaddr*)&localSockAddr, &localSockAddrLength) == -1) {
    logger << "socket=" << socketID_ << " get local address failed: " << strerror(errno) << endlog;
    close();
    return false;
  }
  localIPAddress_ = toIPString_(localSockAddr);
  localPort_ = ntohs(localSockAddr.sin_port);

  sockaddr_in peerSockAddr;
  socklen_t peerSockAddrLength = sizeof(peerSockAddr);
  if (::getpeername(fd_, (sockaddr*)&peerSockAddr, &peerSockAddrLength) == -1) {
    logger << "socket=" << socketID_ << " get peer address failed: " << strerror(errno) << endlog;
    close();
    return false;
  }
  peerIPAddress_ = toIPString_(peerSockAddr);
  peerPort_ = ntohs(peerSockAddr.sin_port);

  logger << "socket=" << socketID_ << " connected local=" << localIPAddress_ << ":" << localPort_ 
         << " peer=" << peerIPAddress_ << ":" << peerPort_ << endlog;
  return 0;
}

}
