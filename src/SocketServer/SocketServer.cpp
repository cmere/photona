#include "include/first.hpp"
#include "FDSelector.hpp"
#include "SocketServer.hpp"
#include "ListenerTCPSocket.hpp"
#include "TCPSocket.hpp"
#include "UDPSocket.hpp"

#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

namespace SocketServer 
{

bool
SocketServer::run(const string& ipaddress, unsigned int port)
{
  pTCPServerSocket_ = make_shared<ListenerTCPSocket>();
  if (!pTCPServerSocket_->bindAndListen(ipaddress, port)) {
    logger << "socket=" << pTCPServerSocket_->getSocketID() << " bind/listen failed: ipaddress=" << ipaddress << " port=" << port << endlog;
    return false;
  }

  if (!pTCPServerSocket_ || !pTCPServerSocket_->isValid()) {
    logger << "socket=" << pTCPServerSocket_->getSocketID() << " is not ready to listen. exit." << endlog;
    return false;
  }

  logger << logger.test << "socket=" << pTCPServerSocket_->getSocketID() 
         << " listening " << pTCPServerSocket_->getLocalIPAddress() << ":" << pTCPServerSocket_->getLocalPort() << endlog;

  pUDPServerSocket_ = make_shared<UDPSocket>();
  if (!pUDPServerSocket_->bind(ipaddress, port)) {
    logger << "error: socket=" << pUDPServerSocket_->getSocketID() << " failed to bind UPD port " << ipaddress << ":" << port << endlog;
    return false;
  }

  FDSelector selector;
  selector.addToReadSelectable(pTCPServerSocket_);
  selector.addToReadSelectable(pUDPServerSocket_);
  selector.addToWriteSelectable(pUDPServerSocket_);
  
  while (1) {
    if (!pTCPServerSocket_->isValid()) {
      logger << "socket=" << pTCPServerSocket_->getSocketID() << " is closed. exit." << endlog;
      return false;
    }

    if (!pUDPServerSocket_->isValid()) {
      logger << "socket=" << pUDPServerSocket_->getSocketID() << " is closed. exit." << endlog;
      return false;
    }

    auto retval = selector.select();
    if (retval <= 0) {
      logger << "socket select errror. exit." << endlog;
      return false;
    }

    // write
    const auto& readyToWriteSockets = selector.getReadyToWrite();
    for (const auto& elm : readyToWriteSockets) {
      if (elm->handleSelectWritable() <= 0 && elm->fd() < 0) {
        logger << logger.test << "remove socket " << elm->getSocketID() << " because write error." << endlog;
        selector.removeFromAll(elm);
        continue;
      }
    }

    // read and accept connection
    const auto& readyToReadSockets = selector.getReadyToRead();
    for (const auto& elm : readyToReadSockets) {
      if (elm->handleSelectReadable() <= 0 && elm->fd() < 0) {
        logger << logger.test << "remove socket " << elm->getSocketID() << " because EOF or read error." << endlog;
        selector.removeFromAll(elm);
        continue;
      }

      if (elm == pTCPServerSocket_) {
        shared_ptr<TCPSocket> pTCPClientSocket = move(pTCPServerSocket_->getAcceptedClient());
        pTCPClientSockets_.insert(pTCPClientSocket);
        selector.addToReadSelectable(pTCPClientSocket);
        selector.addToWriteSelectable(pTCPClientSocket);
      }
    }
  }

  return true;
}

}
