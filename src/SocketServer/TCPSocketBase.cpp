#include "include/first.hpp"
#include "TCPSocketBase.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace SocketServer
{

TCPSocketBase::TCPSocketBase()
{
  fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == -1) {
    logger << "create socket failed [" << strerror(errno) << "]" << endl;
  }
  logger << "socket created fd=" << fd_ << endl;

  /*
  int flag = ::fcntl(fd_, F_GETFD);
  if (flag == -1) {
    logger << "try to set socket NONBLOCK failed. fd=" << fd_ << " " << strerror(errno) << endl;
  }
  flag |= O_NONBLOCK;
  if (::fcntl(fd_, F_SETFD, flag) == -1) {
    logger << "set socket NONBLOCK failed. fd=" << fd_ << " " << strerror(errno) << endl;
  }
  */
}

TCPSocketBase::TCPSocketBase(int fd, const string& peerIPAddress, unsigned int peerPort)
  : fd_(fd), peerIPAddress_(peerIPAddress), peerPort_(peerPort)
{ }

TCPSocketBase::~TCPSocketBase()
{
  close();
}

void 
TCPSocketBase::close()
{
  if (fd_ > 0) {
    ::close(fd_);
    logger << "close fd=" << fd_ << endl;
    fd_ = -1;
  }
}

string
TCPSocketBase::toIPString_(const sockaddr_in& sockaddr)
{
  char buf[INET_ADDRSTRLEN];
  memset(buf, 0, INET_ADDRSTRLEN);
  if (::inet_ntop(AF_INET, &sockaddr.sin_addr.s_addr, buf, INET_ADDRSTRLEN) == NULL) {
    logger << "failed to get IP address " << strerror(errno) << endl;
    return "";
  }
  return string(buf);
}

} // namespace SocketServer


