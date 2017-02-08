#ifndef SOCKETSERVER_LISTENERTCPSOCKET_H
#define SOCKETSERVER_LISTENERTCPSOCKET_H

#include <memory>
#include <string>
#include "TCPSocket.hpp"

namespace SocketServer
{

/**
 * TCP socket bind and listen, accept client connections.
 */
class ListenerTCPSocket : public TCPSocket 
{
  public:
    ListenerTCPSocket();

    virtual int handleSelectReadable();
    virtual int handleSelectWritable();

    bool bindAndListen(const std::string& localIPAddress, unsigned int localPort);

    std::unique_ptr<TCPSocket> getAcceptedClient() { return std::move(pAcceptedClientSocket_); }

  private:
    int totalNumOfAcceptedClientsAfterStart_ = 0;
    std::unique_ptr<TCPSocket> pAcceptedClientSocket_;
};

}

#endif // SOCKETSERVER_LISTENERTCPSOCKET_H
