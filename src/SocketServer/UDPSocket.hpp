#ifndef SOCKETSERVER_UDPSOCKETBASE_H
#define SOCKETSERVER_UDPSOCKETBASE_H

#include <string>
#include "SocketBase.hpp"

struct sockaddr_in;

namespace SocketServer
{

/**
 *
 */
class UDPSocket : public SocketBase
{
  public:
    UDPSocket();

    bool bind(const std::string& localIPAddress, unsigned int localPort);

  private:
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;
};

}

#endif
