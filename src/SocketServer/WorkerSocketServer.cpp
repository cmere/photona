#include "include/first.hpp"
#include "WorkerSocketServer.hpp"
#include "TCPSocket.hpp"

using namespace std;

namespace SocketServer 
{

WorkerSocketServer::WorkerSocketServer(TCPSocket& tcpSocket)
  : tcpSocket_(tcpSocket)
{

}

bool
WorkerSocketServer::run()
{
  return true;
}

}
