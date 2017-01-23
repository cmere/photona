#ifndef WORKERSOCKET_SERVER_HPP
#define WORKERSOCKET_SERVER_HPP

namespace SocketServer
{

class TCPSocket;

/**
 * Worker socket server process requests and responses from one client.
 */
class WorkerSocketServer
{
  public:
    WorkerSocketServer(TCPSocket& socket);

    bool run();

  private:
    TCPSocket& tcpSocket_;
};

}

#endif
