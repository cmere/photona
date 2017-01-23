#ifndef SOCKETSERVER_LISTENSOCKETSERVER_H
#define SOCKETSERVER_LISTENSOCKETSERVER_H

#include <memory>
#include <string>

namespace SocketServer
{

class ListenerTCPSocket;

/**
 * Listener socket server.
 *
 * After accept a connection, it starts a child process acting as worker 
 * socket server. In case child process crash, this listener process will 
 * still be running.
 */
class ListenerSocketServer 
{
  public:
    ListenerSocketServer() { }

    bool listenTo(const std::string& ipaddress, unsigned int port);
    bool run();

  private:
    ListenerSocketServer(const ListenerSocketServer&) = delete;
    ListenerSocketServer& operator=(const ListenerSocketServer&) = delete;

  private:
    std::shared_ptr<ListenerTCPSocket> pListenerTCPSocket_;
};

}

#endif // SOCKETSERVER_LISTENSOCKETSERVER_H
