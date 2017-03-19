#include "include/first.hpp"
#include "FDSelector.hpp"
#include "ISelectable.hpp"
#include "MessageBuffer.hpp"
#include "TCPSocket.hpp"

#include <sys/select.h>
#include <unistd.h>

using namespace std;
using namespace Util;

namespace SocketLib 
{

void 
FDSelector::addToReadSelectable(std::shared_ptr<ISelectable> pSocket)
{
  if (!pSocket || !pSocket->isValid()) {
    return;
  }
  auto& found = readSelectables_[pSocket->getSocketID()];
  if (found) {
    logger << "ignore adding duplicate socket to read fd_set " << pSocket->getSocketID() << endlog;
  }
  else {
    found = pSocket;
  }
}

void 
FDSelector::addToWriteSelectable(std::shared_ptr<ISelectable> pSocket) 
{ 
  if (!pSocket || !pSocket->isValid()) {
    return;
  }
  auto& found = writeSelectables_[pSocket->getSocketID()];
  if (found) {
    logger << "ignore adding duplicate to write fd_set " << pSocket->getSocketID() << endlog;
  }
  else {
    found = pSocket;
  }
}

void 
FDSelector::removeFromAll(std::shared_ptr<ISelectable> pSocket) 
{
  readSelectables_.erase(pSocket->getSocketID());
  writeSelectables_.erase(pSocket->getSocketID());
}

int
setupFDSet(const map<SocketID, shared_ptr<ISelectable>>& pSelectables,
           fd_set& fdset, bool isWriteSelectable)
{
  FD_ZERO(&fdset);
  int maxfd = 0;

  for (auto& elm : pSelectables) {
    auto& pSelectable = elm.second;
    if (pSelectable && pSelectable->isValid()) {
      if (!isWriteSelectable) { // readSelectable
        FD_SET(pSelectable->fd(), &fdset);
      }
      else if (pSelectable->hasBytesToSend()) {
        FD_SET(pSelectable->fd(), &fdset);
      }
      maxfd = max(maxfd, pSelectable->fd());
    }
  }

  return maxfd;
}

int 
FDSelector::select(timeval* timeout) 
{
  fd_set readFDSet;
  fd_set writeFDSet;
  int maxfdRead = setupFDSet(readSelectables_, readFDSet, false);
  int maxfdWrite = setupFDSet(writeSelectables_, writeFDSet, true);
  int maxfd = max(maxfdRead, maxfdWrite);
  if (maxfd == 0) {
    logger << "no more socket to select/pull." << endlog;
    return 0;
  }

  readyToReadSockets_.clear();
  readyToWriteSockets_.clear();

  int numSelected = 0;
  while (1) {  // go back when received signal
    // block wait
    int retval = ::select(maxfd + 1, &readFDSet, &writeFDSet, nullptr, timeout);
    if (retval == -1) {
      if (errno == EINTR) {  // received signal
        logger << "socket select/poll signalled." << strerror(errno) << endlog;
        continue;
      }
      else {
        logger << "socket select/poll error " << strerror(errno) << endlog;
        return -1;
      }
    }
    numSelected = retval;
    break;
  }

  if (numSelected <= 0) {
    logger << "socket nothing selected/polled." << endlog;
  }

  int numSelectedReadSocket = 0;
  // ready to read
  for (auto& elm : readSelectables_) {
    int fd = elm.second->fd();
    if (FD_ISSET(fd, &readFDSet)) {
      readyToReadSockets_.insert(elm.second);
      ++numSelectedReadSocket;
    }
  }
  
  int numSelectedWriteSocket = 0;
  // ready to write
  for (auto& elm : writeSelectables_) {
    int fd = elm.second->fd();
    if (FD_ISSET(fd, &writeFDSet)) {
      readyToWriteSockets_.insert(elm.second);
      ++numSelectedWriteSocket;
    }
  }

  if (numSelected != numSelectedReadSocket + numSelectedWriteSocket) {
    logger << "select/poll FD number not match " << numSelected << " ?=? " << numSelectedReadSocket << " + " << numSelectedWriteSocket << endlog;
  }

  return numSelected;
}

//void addToExceptSelectable(shared_ptr<ISelectable> sock) { }
//std::set<shared_ptr<ISelectable>> getReadyToExcept() const;

}
