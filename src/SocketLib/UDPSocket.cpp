#include "include/first.hpp"
#include "UDPSocket.hpp"
#include "UDPMessageBuffer.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace SocketServer
{
static unsigned int MaxUPDMessageSize = 4096;

UDPSocket::UDPSocket()
  : SocketBase(SOCK_DGRAM) 
{ 
}

bool 
UDPSocket::bind(const std::string& localIPAddress, unsigned int localPort)
{
  if (SocketBase::bind_(localIPAddress, localPort)) {
    logger << logger.test << "UDP socket=" << socketID_ << " bind to " << localIPAddress << ":" << localPort << endlog;
    return true;
  }
  return false;
}

int 
UDPSocket::handleSelectReadable()
{
  if (UDPMessageBuffer::Singleton().shouldReadMoreOnSocket(socketID_)) {
    return 0;
  }

  char buffer[MaxUPDMessageSize];
  sockaddr_in peerSockAddr;
  socklen_t peerSockAddrSize;
  auto msgSize = ::recvfrom(fd_, buffer, MaxUPDMessageSize, 0, (sockaddr*)&peerSockAddr, &peerSockAddrSize);
  if (msgSize < 0) {
    logger << "socket " << socketID_ << " recvfrom failed: " << strerror(errno) << endlog;
    return -1;
  }
  string peerIP = toIPString_(peerSockAddr);
  unsigned int peerPort = ntohs(peerSockAddr.sin_port);
  logger << "socket " << socketID_ << " receive " << msgSize << " bytes from peer " << peerIP << ":" << peerPort << endlog;

  auto extractedSize = UDPMessageBuffer::Singleton().extractMessageFromSocket(buffer, msgSize, socketID_, peerIP, peerPort);

  // if this is a response, delete request from queue

  return extractedSize;
}

int 
UDPSocket::handleSelectWritable()
{
  /*
  auto msgAndPeer = UDPMessageBuffer::Singleton().popMessageToSend(socketID_);
  if (get<0>(msgAndPeer) == 0) {
    logger << "error: socket " << socketID_ << " has no message to send" << endlog;
    return -1;
  }
  auto pMsg = get<0>(msgAndPeer);
  const auto& peerIP = get<1>(msgAndPeer); 
  const auto peerPort = get<2>(msgAndPeer);

  sockaddr_in addr;
  if (!SocketBase::toSockAddr_(peerIP, peerPort, addr)) {
    logger << "error: socket " << socketID_ << " cannot find peer address, no send." << endlog;
    return -1;
  }

  auto bytesAndSize = MessageBase::toBytes(*pMsg);
  auto sentSize = ::sendto(fd_, bytesAndSize.first.get(), bytesAndSize.second, 0, (const sockaddr*)&addr, sizeof(addr));
  logger << "socket " << socketID_ << " sent " << sentSize << " bytes to " << peerIP << ":" << peerPort << endlog;
  if (sentSize <= bytesAndSize.second) {
    logger << "error: not sent whole message " << sentSize << " vs " << bytesAndSize.second << endlog;
    return -1;
  }

  return sentSize;
  */
  return 0;
}

} // namespace SocketServer



