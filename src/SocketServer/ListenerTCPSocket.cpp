#include "include/first.hpp"
#include "ListenerTCPSocket.hpp"

#include <arpa/inet.h>
#include <netinet/ip.h>

using namespace std;

namespace SocketServer
{

ListenerTCPSocket::ListenerTCPSocket()
{
  const int on = 1;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
    logger << "socket option failed fd=" << fd_ << " " << strerror(errno) << endlog;
  }
}

int
ListenerTCPSocket::handleSelectReadable() 
{
  sockaddr_in peerIPSockAddr;
  socklen_t peerIPSockAddrLength = sizeof(peerIPSockAddr);
  int fdPeerSocket = ::accept(fd_, (sockaddr*)&peerIPSockAddr, &peerIPSockAddrLength);
  if (fdPeerSocket == -1) {
    logger << "socket accept error: " << strerror(errno) << endlog;
    return -1;
  }

  string peerIPAddress = toIPString_(peerIPSockAddr);
  unsigned int peerPort = ntohs(peerIPSockAddr.sin_port);
  if (peerIPAddress.empty() || peerPort < 0) {
    logger << "accepted an unknown client [" << peerIPAddress << ":" << peerPort << "]" << endlog;
    return -1;
  }

  pAcceptedClientSocket_.reset(new TCPSocket(fdPeerSocket, peerIPAddress, peerPort));
  logger << logger.test << "accept client socket=" << pAcceptedClientSocket_->getSocketID() << " " << peerIPAddress << ":" << peerPort << endlog;
  
  return 1;
}

int
ListenerTCPSocket::handleSelectWritable() 
{ 
  return 1;
}

bool
ListenerTCPSocket::bindAndListen(const std::string& localIPAddress, unsigned int localPort) 
{
  if (!SocketBase::bind_(localIPAddress, localPort)) {
    return false;
  }

  if (::listen(fd_, 10) == -1) {  // backlog = 10
    logger << "socket failed to listen on fd=" << fd_ << endlog;
    close();
    return false;
  }

  logger << logger.test << "TCP socket=" << socketID_ << " listening " << localIPAddress_ << ":" << localPort_ << endlog;
  return true;
}

}
