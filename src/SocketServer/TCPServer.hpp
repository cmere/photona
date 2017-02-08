#ifndef SOCKETSERVER_TCPSERVER_H
#define SOCKETSERVER_TCPSERVER_H

#include <memory>
#include <string>

namespace SocketServer
{

class ListenerTCPSocket;

/**
 * TCP socket server.
 *
 * After accept a connection, it starts a child process acting as worker 
 * socket server. In case child process crash, this listener process will 
 * still be running.
 */
class TCPServer 
{
  public:
    TCPServer() { }

    bool listenTo(const std::string& ipaddress, unsigned int port);
    bool run();

  private:
    TCPServer(const TCPServer&) = delete;
    TCPServer& operator=(const TCPServer&) = delete;

  private:
    std::shared_ptr<ListenerTCPSocket> pListenerTCPSocket_;
};

}

#endif // SOCKETSERVER_LISTENSOCKETSERVER_H
