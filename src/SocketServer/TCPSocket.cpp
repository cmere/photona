#include "include/first.hpp"
#include "TCPSocket.hpp"
#include "MessageBuffer.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace SocketServer
{


static const unsigned int TCPReadBufferSize = 1024*1024;
static const unsigned int MaxNumberBlocksPerReadWrite = 10;
static const unsigned int MaxInMsgs = 100;

TCPSocket::TCPSocket() 
  : SocketBase(SOCK_STREAM) 
{
}

TCPSocket::TCPSocket(int fd, const std::string& peerIPAddress, unsigned int peerPort)
  : SocketBase(fd, peerIPAddress, peerPort) 
{
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
  logger << "select readable" << endlog;
  /*
  char bytes_[TCPReadBufferSize];
  unsigned int totalNumBytesRead = 0;
  while (totalNumBytesRead < MaxNumberOfRead * 1024 * 1024) {
    int numBytesRead = ::read(fd_, bytes_, TCPReadBufferSize);
    if (numBytesRead == 0) {
      logger << "socket=" << socketID_ << " read EOF" << endlog;
      close();
      break;
    }
    else if (numBytesRead < 0) {
      if (errno != EAGAIN) {
        logger << "socket=" << socketID_ << " read error " << strerror(errno) << endlog;
        close();
      }
      break;
    }
    //vecBuffer_.insert(vecBuffer_.end(), bytes_, bytes_ + numBytesRead);
    //strBuffer_.append(bytes_, numBytesRead);
    dequeBuffer_.insert(dequeBuffer_.end(), bytes_, bytes_ + numBytesRead);
    totalNumBytesRead += numBytesRead;
    logger << "read bytes " << numBytesRead << " " << totalNumBytesRead << endlog;
  }

  // parse 
  //unsigned int numExtractedBytes = totalNumBytesRead / 2;
  //strBuffer_ = strBuffer_.substr(numExtractedBytes);

  return totalNumBytesRead;
  */


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
      logger << "socket=" << socketID_ << " read EOF" << endlog;
      close();
      break;
    }
    else if (numBytesRead < 0) {
      if (errno != EAGAIN) {
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

  /*
  int totalNumBytesRead = 0;
  while (MessageBuffer::Singleton().shouldReadMoreOnSocket(socketID_)) {
    unsigned int bufferSize = recvBufferPtrEnd_ == nullptr ? recvBufferCapacity_  : recvBufferCapacity_ - (recvBufferPtrEnd_ - recvBuffer_.get());
    if (bufferSize == 0) {
      if (recvBufferPtrBegin_ == recvBuffer_.get()) {
        // expand buffer
        unique_ptr<char[]> oldBuffer = move(recvBuffer_);
        recvBuffer_.reset(new char[recvBufferCapacity_ * 2]);
        ::memcpy(recvBuffer_.get(), oldBuffer.get(), recvBufferCapacity_);
        recvBufferPtrBegin_ = recvBuffer_.get();
        recvBufferPtrEnd_ = recvBuffer_.get() + recvBufferCapacity_;
        bufferSize = recvBufferCapacity_;
        recvBufferCapacity_ *= 2;
        logger << "buffer expand " << recvBufferCapacity_ << endlog;
      }
      else {
        // move data to the begining
        unsigned int length = recvBufferPtrEnd_ - recvBufferPtrBegin_;
        ::memcpy(recvBuffer_.get(), recvBufferPtrBegin_, length);
        recvBufferPtrBegin_ = recvBuffer_.get();
        recvBufferPtrEnd_ = recvBuffer_.get() + length;
        bufferSize = recvBufferCapacity_ - length;
        logger << "buffer move data " << length << endlog;
      }
    }

    int numBytesRead = ::read(fd_, recvBufferPtrEnd_, bufferSize);
    if (numBytesRead == 0) {
      logger << "socket=" << socketID_ << " read EOF" << endlog;
      close();
      return 0;
    }
    if (numBytesRead < 0) {
      if (errno != EAGAIN) {
        logger << "socket=" << socketID_ << " read error " << strerror(errno) << endlog;
        close();
      }
      break;
    }
    if (recvBufferPtrBegin_ == nullptr) {
      recvBufferPtrBegin_ = recvBuffer_.get();
    }
    recvBufferPtrEnd_ += numBytesRead;
    totalNumBytesRead += numBytesRead;
    logger << "read bytes " << numBytesRead << " " << totalNumBytesRead << endlog;
  }
  return totalNumBytesRead;
  */

  /*
  logger << "select readable" << endlog;
  unique_ptr<char> bytes(new char[TCPReadBufferSize]);
  unsigned int bufferSize = TCPReadBufferSize;
  int numBytesRead = 0;
  int numBytesRead = 1;
  while (numBytesRead != 0) {
    numBytesRead = ::read(fd_, bytes.get(), bufferSize);
    if (numBytesRead < 0 && errno == EAGAIN) {
    }
    else {
      logger << "read bytes " << numBytesRead << endlog;
    }
  }
  return numBytesRead;
  */

  /*
  if (MessageBuffer::Singleton().shouldReadMoreOnSocket(socketID_)) {
    // copy left-over from last read().
    if (numBytesNotExtracted_ > 0) {
      if (numBytesNotExtracted_ * 2 >= TCPReadBufferSize) {  // for a large message, increase bytes[] buffer.
        bufferSize = TCPReadBufferSize;
        while (numBytesNotExtracted_ * 2 >= bufferSize) {
          bufferSize *= 2;
        }
        bytes.reset(new char[bufferSize]);
      }
      ::memcpy(bytes.get(), pBytesNotExtracted_.get(), numBytesNotExtracted_);
      pBytesNotExtracted_.reset();
    }

    numBytesRead = ::read(fd_, bytes.get() + numBytesNotExtracted_, bufferSize - numBytesNotExtracted_);
    logger << "socket=" << socketID_ << " read " << numBytesRead << " bytes" << endlog;
    if (numBytesRead == -1) {
      logger << "socket=" << socketID_ << " read error " << strerror(errno) << endlog;
      close();
      return -1;
    }
    else if (numBytesRead == 0) {  // eof
      logger << logger.test << "socket=" << socketID_ << " EOF" << endlog;
      close();
      return 0;
    }

    char* pMessageBoundary = bytes.get();
    unsigned int numBytes = numBytesNotExtracted_ + (unsigned int)numBytesRead;

    while (MessageBuffer::Singleton().shouldReadMoreOnSocket(socketID_)) {
      unsigned int numBytesExtracted = MessageBuffer::Singleton().extractMessageFromSocket(pMessageBoundary, numBytes, socketID_);
      if (numBytesExtracted == 0 ) {  // no more WHOLE message.
        numBytesNotExtracted_ = numBytes;
        pBytesNotExtracted_.reset(new char[numBytesNotExtracted_]);
        ::memcpy(pBytesNotExtracted_.get(), pMessageBoundary, numBytesNotExtracted_);
        break;
      }
      pMessageBoundary += numBytesExtracted;
      numBytes -= numBytesExtracted;

      if (numBytes <= 0) {  // all bytes are extracted into messages.
        numBytesNotExtracted_ = 0;
        pBytesNotExtracted_.reset();
        break;
      }
    }

    // save not extracted bytes which are: 1) partial message; or 2) buffer is full and decide not read more.
    if (numBytes > 0) {
      numBytesNotExtracted_ = numBytes;
      pBytesNotExtracted_.reset(new char[numBytesNotExtracted_]);
      ::memcpy(pBytesNotExtracted_.get(), pMessageBoundary, numBytesNotExtracted_);
    }
  }

  return numBytesRead;
*/
}

int
TCPSocket::handleSelectWritable()
{
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

  /*
  // send left-over from last send().
  char* pCurrent = pBytesNotSend_.get();
  if (numBytesNotSend_ > 0) {
    ssize_t numBytesSent = ::write(fd_, pCurrent, numBytesNotSend_);
    if (numBytesSent == -1) {
      logger << "socket=" << socketID_ << " write error" << strerror(errno) << endlog;
      return -1;
    }
    else if (numBytesSent == 0) { // nothing was written, shouldn't happen
      logger << "ERROR: should not write zero bytes. socket=" << socketID_ << endlog;
    }
    else {
      numBytesNotSend_ -= numBytesSent;
      pCurrent += numBytesSent;
    }
    if (numBytesNotSend_ > 0) {
      pBytesNotSend_.reset(new char[numBytesNotSend_]);
      ::memcpy(pBytesNotSend_.get(), pCurrent, numBytesNotSend_);
    }

    logger << logger.test << "socket=" << socketID_ << " wrote " << numBytesSent << " bytes" << endlog;
    return numBytesSent;
  }

  // numBytesNotSend_ == 0
  pBytesNotSend_.reset();

  if (MessageBuffer::Singleton().hasMessageToSend(socketID_)) {
    shared_ptr<MessageBase> pMsg = MessageBuffer::Singleton().popMessageToSend(socketID_);
    pair<unique_ptr<char>, unsigned int> bytes_length = MessageBase::toBytes(*pMsg);

    char* pCurrent = bytes_length.first.get();
    unsigned int numBytes = bytes_length.second;

    if (numBytes <= 0) {
      logger << "failed to get bytes from message socket=" << socketID_ << " " << *pMsg << endlog;
      return 0;
    }

    logger << "begin write" << endlog;
    while (numBytes > 0) {
      int len = ::write(fd_, pCurrent, numBytes);
      pCurrent += len;
      numBytes -= len;
      logger << "write " << len << " bytes" << endlog;
    }
    ssize_t numBytesSent = ::write(fd_, pCurrent, numBytes);
    if (numBytesSent == -1) {
      logger << "socket write error. socket=" << socketID_ << " " << strerror(errno) << endlog;
      return -1;
    }
    else if (numBytesSent == 0) { // shouldn't happen
      logger << "ERROR: should not write zero bytes. socket=" << socketID_ << endlog;
    }
    else {
      numBytes -= numBytesSent;
      pCurrent += numBytesSent;
    }

    if (numBytes > 0) {
      numBytesNotSend_ = numBytes;
      pBytesNotSend_.reset(new char[numBytesNotSend_]);
      ::memcpy(pBytesNotSend_.get(), pCurrent, numBytesNotSend_);
    }
    logger << logger.test << "socket=" << socketID_ << " wrote " << numBytesSent << " bytes" << endlog;
    return numBytesSent;
  }

  return 0;
  */
}

bool
TCPSocket::connectTo(const string& serverIPAddress, unsigned int serverPort)
{
  sockaddr_in peerSockAddr;
  socklen_t peerSockAddrLength = sizeof(peerSockAddr);
  peerSockAddr.sin_family= AF_INET;
  peerSockAddr.sin_port = htons(serverPort);
  inet_pton(AF_INET, serverIPAddress.c_str(), &peerSockAddr.sin_addr.s_addr);

  if (::connect(fd_, (const sockaddr*)&peerSockAddr, peerSockAddrLength) == -1) {
    logger << "socket=" << socketID_ << " connect failed: " << strerror(errno) << endlog;
    return false;
  }
  
  // set local/peer address
  peerIPAddress_ = serverIPAddress;
  peerPort_ = serverPort;

  sockaddr_in localSockAddr;
  socklen_t localSockAddrLength = sizeof(localSockAddr);
  if (::getsockname(fd_, (sockaddr*)&localSockAddr, &localSockAddrLength) == -1) {
    logger << "socket=" << socketID_ << " get address failed: " << strerror(errno) << endlog;
    close();
    return false;
  }

  localIPAddress_ = toIPString_(localSockAddr);
  localPort_ = ntohs(localSockAddr.sin_port);

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

}
