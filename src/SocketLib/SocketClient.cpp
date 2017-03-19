#include "include/first.hpp"
#include "SocketClient.hpp"
#include "FDSelector.hpp"
#include "MessageCenter.hpp"
#include "TCPSocket.hpp"

using namespace std;
using namespace Util;

namespace SocketLib 
{

void
SocketClient::addTCPSocket(std::shared_ptr<TCPSocket> pSocket)
{
  pSockets_.insert(make_pair(pSocket->getSocketID(), pSocket));
}

bool
SocketClient::run()
{
  FDSelector selector;
  for (auto elm: pSockets_) {
    selector.addToReadSelectable(elm.second);
    selector.addToWriteSelectable(elm.second);
  }
  selector.addToReadSelectable(MessageCenter::GetSharedPtr());
  
  while (selector.getNumberSelectables() > 1) {  // MessageCenter's ISelectable is always in selector
    auto retval = selector.select();
    if (retval <= 0) {
      logger << "socket select errror. exit." << endlog;
      return false;
    }

    // write
    const auto& readyToWriteSockets = selector.getReadyToWrite();
    for (const auto& elm : readyToWriteSockets) {
      if (elm->handleSelectWritable() < 0 && elm->fd() < 0) {
        logger << logger.test << "selector remove socket=" << elm->getSocketID() << " because write error." << endlog;
        selector.removeFromAll(elm);
        continue;
      }
    }

    // read 
    const auto& readyToReadSockets = selector.getReadyToRead();
    for (const auto& elm : readyToReadSockets) {
      if (elm->handleSelectReadable() <= 0 && elm->fd() < 0) {
        logger << logger.test << "selector remove finished socket=" << elm->getSocketID() << endlog;
        selector.removeFromAll(elm);
        continue;
      }
    }

    for (auto socketID : MessageCenter::GetSharedPtr()->getFinishedSocketIDs()) {
      auto found = pSockets_.find(socketID);
      if (found != pSockets_.end()) {
        selector.removeFromAll(found->second);  // remove before closing socket, after close fd=-1.
        found->second->close();
        pSockets_.erase(found);
        logger << logger.test << "selector remove finished socket=" << socketID << endlog;
      }
    }
    MessageCenter::GetSharedPtr()->clearFinishedSocketIDs();
  }

  return true;
}

}
