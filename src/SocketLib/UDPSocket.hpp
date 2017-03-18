#ifndef SOCKETLIB_UDPSOCKETBASE_H
#define SOCKETLIB_UDPSOCKETBASE_H

#include <string>
#include "SocketBase.hpp"

struct sockaddr_in;

namespace SocketLib
{

/**
 *
 */
class UDPSocket : public SocketBase
{
  public:
    UDPSocket();

    bool bind(const std::string& localIPAddress, unsigned int localPort);

    virtual int handleSelectReadable();
    virtual int handleSelectWritable();
    virtual bool hasBytesToSend() const { return false; }

  private:
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;
};

}

#endif
