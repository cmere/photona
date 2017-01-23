#include "include/first.hpp"
#include "TCPSocket.hpp"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace SocketServer
{

TCPSocket::TCPSocket()
{
  fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == -1) {
    logger << "create socket failed [" << strerror(errno) << "]" << endl;
  }
  logger << "create fd=" << fd_ << endl;
}

TCPSocket::TCPSocket(int fd, const string& peerClientAddr, unsigned int peerClientPort)
  : fd_(fd), peerIPAddress_(peerClientAddr), peerPort_(peerClientPort)
{
}

TCPSocket::~TCPSocket()
{
  close();
}

bool 
TCPSocket::handleSelectReadable()
{
  return false;
}

bool 
TCPSocket::handleSelectWritable()
{
  return false;
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

void 
TCPSocket::close()
{
  if (fd_ > 0) {
    ::close(fd_);
    logger << "close fd=" << fd_ << endl;
    fd_ = -1;
  }
}

string
TCPSocket::toIPString_(const sockaddr_in& sockaddr)
{
  char buf[INET_ADDRSTRLEN];
  memset(buf, 0, INET_ADDRSTRLEN);
  if (::inet_ntop(AF_INET, &sockaddr.sin_addr.s_addr, buf, INET_ADDRSTRLEN) == NULL) {
    logger << "failed to get IP address " << strerror(errno) << endl;
    return "";
  }
  return string(buf);
}

}
