#include "include/first.hpp"
#include "MessageBuffer.hpp"
#include "BlockBuffer.hpp"
#include "MessageBase.hpp"

#include <unistd.h>

using namespace std;
using namespace Util;

namespace SocketLib
{

MessageBuffer&
MessageBuffer::Singleton()
{
  static MessageBuffer theObject;
  return theObject;
}

MessageBuffer::MessageBuffer()
{
  int pipefd[2];
  if (::pipe(pipefd) == -1) {
    logger << logger.fatal << "MessageBuffer failed to open pipe: " << strerror(errno) << endlog;
  }
  fdInMsgPipeRead_ = pipefd[0];
  fdInMsgPipeWrite_ = pipefd[1];
}

bool
MessageBuffer::hasMessageToSend(const SocketID& socketID) const
{
  return outMsgBySocketID_.count(socketID) > 0;
}

bool 
MessageBuffer::queueMessageToSend(const std::shared_ptr<MessageBase>& pMsg, const SocketID& socketID)
{
  if (pMsg) {
    queueOut_.push_back(make_pair(socketID, pMsg));
    outMsgBySocketID_[socketID].push_back(--queueOut_.end());
    logger << logger.test << "socket=" << socketID << " queue message " << pMsg->getName() << endlog;
  }
  return true;
}

pair<SocketID, shared_ptr<MessageBase>>
MessageBuffer::popFirstMessageInQueue()
{
  auto itBegin = queueIn_.begin();
  auto socketID_pMsg = *itBegin;
  auto found = inMsgBySocketID_.find(socketID_pMsg.first);
  if (found != inMsgBySocketID_.end()) {
    for (auto it = found->second.begin(); it != found->second.end(); ++it) {
      if (*it == itBegin) {
        found->second.erase(it);
        break;
      }
    }
    if (found->second.empty()) {
      inMsgBySocketID_.erase(found);
    }
  }

  queueIn_.pop_front();
  return socketID_pMsg;
}

shared_ptr<MessageBase> 
MessageBuffer::popMessageToSend(const SocketID& socketID)
{
  shared_ptr<MessageBase> pMsg;
  auto found = outMsgBySocketID_.find(socketID);
  if (found != outMsgBySocketID_.end()) {
    auto& itorList = found->second;
    if (!itorList.empty()) {
      auto itor = *itorList.begin();
      pMsg = itor->second;
      queueOut_.erase(itor);
      itorList.erase(itorList.begin());
      if (itorList.empty()) {
        outMsgBySocketID_.erase(found);
      }
    }
  }
  return pMsg;
}

unsigned int
MessageBuffer::extractMessageFromSocket(BlockBuffer& blockBuffer, const SocketID& socketID)
{
  shared_ptr<MessageBase> pMsg;
  int numBytesExtracted = MessageBase::fromBytes(blockBuffer, pMsg);
  if (!pMsg) {
    if (numBytesExtracted < 0) {
      logger << "socket=" << socketID << " failed to extract a message." << endlog;
    }
    return numBytesExtracted;
  }
  queueIn_.push_back(make_pair(socketID, pMsg));
  inMsgBySocketID_[socketID].push_back(--queueIn_.end());
  logger << logger.test << "socket=" << socketID << " extracted " << pMsg->getName() << " (" << numBytesExtracted << " bytes)" << endlog;
  
  // through pipe, notify another end that there is an incoming message.
  char c = '1';
  if (::write(fdInMsgPipeWrite_, &c, sizeof(c)) == -1) {
    logger << "socketID=" << socketID << " faild to notify pipe, delete message in buffer " << pMsg->getName() << endlog;
    queueIn_.pop_back();
    inMsgBySocketID_[socketID].pop_back();
  }

  return numBytesExtracted;
}

unsigned int
MessageBuffer::getNumBufferedMessages(const SocketID& socketID) const
{
  const auto& inMsgs = inMsgBySocketID_.find(socketID);
  if (inMsgs != inMsgBySocketID_.end()) {
    return inMsgs->second.size();
  }
  return 0;
}

}
