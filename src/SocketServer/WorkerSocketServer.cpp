#include "include/first.hpp"
#include "FDSelector.hpp"
#include "TCPSocket.hpp"
#include "WorkerSocketServer.hpp"

using namespace std;

namespace SocketServer 
{

WorkerSocketServer::WorkerSocketServer(shared_ptr<TCPSocket> pTcpSocket)
  : pTcpSocket_(pTcpSocket)
{

}

bool
WorkerSocketServer::run()
{
  FDSelector selector;
  selector.addToReadSelectable(pTcpSocket_);
  selector.addToWriteSelectable(pTcpSocket_);

  while (1) {
    int retval = selector.select();
    if (retval < 0) {
      logger << "socket select errror. exit." << endl;
      return false;
    }
    else if (retval == 0) {
      logger << "worker socket server exit." << endl;
      return true;
    }

    if (!selector.getReadyToRead().empty()) {
      pTcpSocket_->handleSelectReadable();
    }
    if (!selector.getReadyToWrite().empty()) {
      pTcpSocket_->handleSelectWritable();
    }
  }

  return true;
}

}
