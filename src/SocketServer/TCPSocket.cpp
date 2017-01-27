#include "include/first.hpp"
#include "TCPSocket.hpp"
#include "MessageBuffer.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

const unsigned int TCPReadBufferSize = 8096;

namespace SocketServer
{

void 
TCPSocket::close()
{
  MessageBuffer::Singleton().removeSocketMessages(getPeerPair());
  TCPSocketBase::close();
}

int
TCPSocket::handleSelectReadable()
{
  unique_ptr<char> bytes(new char[TCPReadBufferSize]);
  unsigned int bufferSize = TCPReadBufferSize;
  int numBytesRead = 0;

  if (MessageBuffer::Singleton().needReadMore(getPeerPair())) {
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
    if (numBytesRead == -1) {
      logger << "socket read error fd=" << fd_ << " " << strerror(errno) << endl;
      close();
      return -1;
    }
    else if (numBytesRead == 0) {  // eof
      logger << "close socket fd=" << fd_ << endl;
      close();
      return 0;
    }

    char* pMessageBoundary = bytes.get();
    unsigned int numBytes = numBytesNotExtracted_ + (unsigned int)numBytesRead;

    while (MessageBuffer::Singleton().needReadMore(getPeerPair())) {
      unsigned int numBytesExtracted = MessageBuffer::Singleton().extractMessageFromBytes(pMessageBoundary, numBytes);
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

    // save not extracted bytes which are: 1) partial message; 2) buffer is full and decide not read more.
    if (numBytes > 0) {
      numBytesNotExtracted_ = numBytes;
      pBytesNotExtracted_.reset(new char[numBytesNotExtracted_]);
      ::memcpy(pBytesNotExtracted_.get(), pMessageBoundary, numBytesNotExtracted_);
    }
  }

  return numBytesRead;
}

int
TCPSocket::handleSelectWritable()
{
  // send as many bytes as possible. If receiver side is choked, select() won't set writable on this socket.

  // send left-over from last send().
  char* pCurrent = pBytesNotSend_.get();
  if (numBytesNotSend_ > 0) {
    ssize_t numBytesSent = ::write(fd_, pCurrent, numBytesNotSend_);
    if (numBytesSent == -1) {
      logger << "socket write error. fd=" << fd_ << " " << strerror(errno) << endl;
      return -1;
    }
    else if (numBytesSent == 0) { // nothing was written, shouldn't happen
      logger << "ERROR: should not write zero bytes. fd=" << fd_ << endl;
    }
    else {
      numBytesNotSend_ -= numBytesSent;
      pCurrent += numBytesSent;
    }
    if (numBytesNotSend_ > 0) {
      pBytesNotSend_.reset(new char[numBytesNotSend_]);
      ::memcpy(pBytesNotSend_.get(), pCurrent, numBytesNotSend_);
    }

    return numBytesSent;
  }

  // numBytesNotSend_ == 0
  pBytesNotSend_.reset();

  if (MessageBuffer::Singleton().hasMessageToSend(getPeerPair())) {
    shared_ptr<MessageBase> msg = MessageBuffer::Singleton().popMessage(getPeerPair());
    pair<unique_ptr<char>, unsigned int> bytes_length = msg->toBytes();

    char* pCurrent = bytes_length.first.get();
    unsigned int numBytes = bytes_length.second;

    if (numBytes <= 0) {
      logger << "failed to get bytes from message fd=" << fd_ << " " << *msg << endl;
      return 0;
    }

    ssize_t numBytesSent = ::write(fd_, pCurrent, numBytes);
    if (numBytesSent == -1) {
      logger << "socket write error. fd=" << fd_ << " " << strerror(errno) << endl;
      return -1;
    }
    else if (numBytesSent == 0) { // shouldn't happen
      logger << "ERROR: should not write zero bytes. fd=" << fd_ << endl;
    }
    else {
      numBytes -= numBytesSent;
      pCurrent += numBytesSent;
    }

    if (numBytes > 0) {
      pBytesNotSend_.reset(new char[numBytes]);
      ::memcpy(pBytesNotSend_.get(), pCurrent, numBytes);
    }
    return numBytesSent;
  }

  return 0;
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
    cout << "socket connect failed: fd=" << fd_ << " " << strerror(errno) << endl;
    return false;
  }
  
  // set local/peer address
  peerIPAddress_ = serverIPAddress;
  peerPort_ = serverPort;

  sockaddr_in localSockAddr;
  socklen_t localSockAddrLength = sizeof(localSockAddr);
  if (::getsockname(fd_, (sockaddr*)&localSockAddr, &localSockAddrLength) == -1) {
    logger << "socket address failed fd=" << fd_ << " " << strerror(errno) << endl;
    close();
    return false;
  }

  localIPAddress_ = toIPString_(localSockAddr);
  localPort_ = ntohs(localSockAddr.sin_port);

  return true;
}

}
