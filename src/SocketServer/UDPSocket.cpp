#include "include/first.hpp"
#include "UDPSocket.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace SocketServer
{

UDPSocket::UDPSocket()
  : SocketBase(SOCK_DGRAM) 
{ 
}

bool 
UDPSocket::bind(const std::string& localIPAddress, unsigned int localPort)
{
  return SocketBase::bind_(localIPAddress, localPort);
}

} // namespace SocketServer



