#include "include/first.hpp"
#include "SocketBase.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace Util;

namespace SocketLib
{

std::string SocketBase::ANY_IPADDRESS = "*";
unsigned int SocketBase::RANDOM_PORT = 0;
SocketID SocketBase::staticSocketID_ = 0;

SocketBase::SocketBase(int socketType)
  : socketID_(staticSocketID_++)
{
  fd_ = ::socket(AF_INET, socketType, 0);
  if (fd_ == -1) {
    logger << "create socket=" << socketID_ << " failed [" << strerror(errno) << "]" << endlog;
    return;
  }
  logger << "create socket=" << socketID_ << " fd=" << fd_ << endlog;

  /*
  int flag = ::fcntl(fd_, F_GETFD);
  if (flag == -1) {
    logger << "try to set socket NONBLOCK failed. fd=" << fd_ << " " << strerror(errno) << endlog;
  }
  flag |= O_NONBLOCK;
  if (::fcntl(fd_, F_SETFD, flag) == -1) {
    logger << "set socket NONBLOCK failed. fd=" << fd_ << " " << strerror(errno) << endlog;
  }
  */
}

SocketBase::SocketBase(int fd, const string& peerIPAddress, unsigned int peerPort)
  : socketID_(staticSocketID_++), fd_(fd), peerIPAddress_(peerIPAddress), peerPort_(peerPort)
{
  logger << "create socket=" << socketID_ << " fd=" << fd_ << " peer=" << peerIPAddress << ":" << peerPort << endlog;
}

SocketBase::~SocketBase()
{
  logger << "dtor socket " << socketID_ << endlog;
  close();
}

void 
SocketBase::close()
{
  if (fd_ > 0) {
    ::close(fd_);
    logger << "close socket " << socketID_ << " fd=" << fd_ << endlog;
    fd_ = -1;
  }
}

string
SocketBase::toIPString_(const sockaddr_in& sockaddr)
{
  char buf[INET_ADDRSTRLEN];
  memset(buf, 0, INET_ADDRSTRLEN);
  if (::inet_ntop(AF_INET, &sockaddr.sin_addr.s_addr, buf, INET_ADDRSTRLEN) == NULL) {
    logger << "failed to get IP address " << strerror(errno) << endlog;
    return "";
  }
  return string(buf);
}

bool 
SocketBase::toSockAddr_(const std::string& ip, unsigned int port, sockaddr_in& addr)
{
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  int retval = ::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);
  if (retval <= 0) {
    logger << "error: convert ip address [" << ip << "] " << strerror(errno) << endlog;
    return false;
  }
  return true;
}


bool
SocketBase::bind_(const std::string& localIPAddress, unsigned int localPort) 
{
  sockaddr_in localSockAddr;
  memset(&localSockAddr, 0, sizeof(localSockAddr));
  localSockAddr.sin_family = AF_INET;
  localSockAddr.sin_port = htons(localPort);
  if (localIPAddress == ANY_IPADDRESS) {
    localSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // OS select address. TODO: for UDP, bind to each IP if multihomed host.
  }
  else {
    // convert string format, e.g. "192.168.1.100", to network format.
    int retval = inet_pton(AF_INET, localIPAddress.c_str(), &localSockAddr.sin_addr.s_addr);
    if (retval <= 0) {
      logger << "error: convert ip address [" << localIPAddress << "] " << strerror(errno) << endlog;
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

  return true;
}

} // namespace SocketLib


