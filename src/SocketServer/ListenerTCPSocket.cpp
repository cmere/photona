#include "include/first.hpp"
#include "ListenerTCPSocket.hpp"

#include <arpa/inet.h>
#include <netinet/ip.h>

using namespace std;

namespace SocketServer
{

std::string ListenerTCPSocket::ANY_IPADDRESS = "*";
unsigned int ListenerTCPSocket::RANDOM_PORT = 0;

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
  int fdPeerSocket = ::accept4(fd_, (sockaddr*)&peerIPSockAddr, &peerIPSockAddrLength, SOCK_NONBLOCK | SOCK_CLOEXEC);
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
  logger << "new client fd=" << fdPeerSocket << " " << peerIPAddress << ":" << peerPort << endlog;
  
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
  sockaddr_in localSockAddr;
  memset(&localSockAddr, 0, sizeof(localSockAddr));
  localSockAddr.sin_family = AF_INET;
  localSockAddr.sin_port = htons(localPort);
  if (localIPAddress == ANY_IPADDRESS) {
    localSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // OS select address
  }
  else {
    // convert string format, e.g. "192.168.1.100", to network format.
    int retval = inet_pton(AF_INET, localIPAddress.c_str(), &localSockAddr.sin_addr.s_addr);
    if (retval == 0) {
      logger << "invalid ip address: " << localIPAddress << endlog;
      close();
      return false;
    }
    else if (retval == -1) {  // not support AF_INET
      logger << "invalid address family: " << strerror(errno) << endlog;
      close();
      return false;
    }
  }
  
  if (::bind(fd_, (const sockaddr*)&localSockAddr, sizeof(localSockAddr)) == -1) {
    logger << "socket bind failed fd=" << fd_ << " " << localIPAddress << ":" << localPort << " " << strerror(errno) << endlog;
    close();
    return false;
  }

  // get local ip address and port in case bind to a kernel chosen ephemeral port.
  socklen_t localSockAddrLength = sizeof(localSockAddr);
  if (::getsockname(fd_, (sockaddr*)&localSockAddr, &localSockAddrLength) == -1) {
    logger << "socket address failed fd=" << fd_ << " " << strerror(errno) << endlog;
    close();
    return false;
  }

  setLocalPort_(ntohs(localSockAddr.sin_port));

  // if bind to INADDR_ANY, the kernel does not choose local IP address until 
  // either the socket is connected (TCP) or datagram is sent on the socket (UDP).
  if (localIPAddress != ANY_IPADDRESS) {
    string ipAddress = toIPString_(localSockAddr);
    if (ipAddress.empty() || ipAddress != localIPAddress) {  // double check
      logger << "after bind, server IP address is failed [" << ipAddress << "]" << endlog;
      close();
      return false;
    }
    setLocalIPAddress_(ipAddress);
  }
  else {
    setLocalIPAddress_("*");
  }


  if (::listen(fd_, 10) == -1) {  // backlog = 10
    logger << "socket failed to listen on fd=" << fd_ << endlog;
    close();
    return false;
  }

  return true;
}

}
