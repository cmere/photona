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
    WorkerSocketServer(std::shared_ptr<TCPSocket>);

    bool run();

  private:
    std::shared_ptr<TCPSocket> pTcpSocket_;
};

}

#endif
