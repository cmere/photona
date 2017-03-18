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
  auto& found = readSelectableByFD_[pSocket->fd()];
  if (found) {
    logger << "add duplicate to read fd_set " << pSocket->fd() << endlog;
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
  auto& found = writeSelectableByFD_[pSocket->fd()];
  if (found) {
    logger << "add duplicate to write fd_set " << pSocket->fd() << endlog;
  }
  else {
    found = pSocket;
  }
}

void 
FDSelector::removeFromAll(std::shared_ptr<ISelectable> pSocket) 
{
  readSelectableByFD_.erase(pSocket->fd());
  writeSelectableByFD_.erase(pSocket->fd());
}

int
setupFDSet(const map<int, shared_ptr<ISelectable>>& pSelectableByFD,
           fd_set& fdset, bool isWriteSelectable)
{
  FD_ZERO(&fdset);
  int maxfd = 0;

  for (auto& elm : pSelectableByFD) {
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
  int maxfdRead = setupFDSet(readSelectableByFD_, readFDSet, false);
  int maxfdWrite = setupFDSet(writeSelectableByFD_, writeFDSet, true);
  int maxfd = max(maxfdRead, maxfdWrite);
  if (maxfd == 0) {
    logger << "no more socket to select/pull." << endlog;
    return 0;
  }

  int numSelectedFDs = 0;
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
    numSelectedFDs = retval;
    break;
  }

  readyToReadFDs_.clear();
  readyToWriteFDs_.clear();

  if (numSelectedFDs <= 0) {
    logger << "socket nothing selected/polled." << endlog;
  }

  int numSelectedReadSocket = 0;
  // ready to read
  for (auto& elm : readSelectableByFD_) {
    int fd = elm.first;
    if (FD_ISSET(fd, &readFDSet)) {
      readyToReadFDs_.insert(fd);
      ++numSelectedReadSocket;
    }
  }
  
  int numSelectedWriteSocket = 0;
  // ready to write
  for (auto& elm : writeSelectableByFD_) {
    int fd = elm.first;
    if (FD_ISSET(fd, &writeFDSet)) {
      readyToWriteFDs_.insert(fd);
      ++numSelectedWriteSocket;
    }
  }

  if (numSelectedFDs != numSelectedReadSocket + numSelectedWriteSocket) {
    logger << "select/poll FD number not match " << numSelectedFDs << " ?=? " << numSelectedReadSocket << " + " << numSelectedWriteSocket << endlog;
  }

  return numSelectedFDs;
}

set<shared_ptr<ISelectable>> 
FDSelector::getReadyToRead() const
{
  set<shared_ptr<ISelectable>> retval;
  for (auto fd: readyToReadFDs_) {
    auto found = readSelectableByFD_.find(fd);
    if (found != readSelectableByFD_.end() && found->second) {
      retval.insert(found->second);
    }
  }
  return retval;
}


set<shared_ptr<ISelectable>> 
FDSelector::getReadyToWrite() const
{
  set<shared_ptr<ISelectable>> retval;
  for (auto fd: readyToWriteFDs_) {
    auto found = writeSelectableByFD_.find(fd);
    if (found != writeSelectableByFD_.end() && found->second) {
      retval.insert(found->second);
    }
  }
  return retval;
}


//void addToExceptSelectable(shared_ptr<ISelectable> sock) { }
//std::set<shared_ptr<ISelectable>> getReadyToExcept() const;

}
