#ifndef SOCKETSERVER_SOCKETSERVER_H
#define SOCKETSERVER_SOCKETSERVER_H

#include <set>
#include <memory>
#include <string>

namespace SocketServer
{

class ListenerTCPSocket;
class TCPSocket;
class UDPSocket;

/**
 * Socket server. Serve TCP and UPD on the same port number. single process, single thread.
 */
class SocketServer 
{
  public:
    bool run(const std::string& ipaddress, unsigned int port);

  private:
    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;

  private:
    std::shared_ptr<UDPSocket> pUDPServerSocket_;

    std::shared_ptr<ListenerTCPSocket> pTCPServerSocket_;
    std::set<std::shared_ptr<TCPSocket>> pTCPClientSockets_;
};

}

#endif // SOCKETSERVER_LISTENSOCKETSERVER_H
