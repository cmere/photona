#ifndef SOCKETLIB_SOCKETLIB_H
#define SOCKETLIB_SOCKETLIB_H

#include <set>
#include <memory>
#include <string>

namespace SocketLib
{

class ListenerTCPSocket;
class TCPSocket;
class UDPSocket;

/**
 * Socket server. Serve TCP and UPD on the same port number.
 */
class SocketServer
{
  public:
    SocketServer() { }

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

#endif // SOCKETLIB_LISTENSOCKETLIB_H
