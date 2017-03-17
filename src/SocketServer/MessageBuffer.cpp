#include "include/first.hpp"
#include "MessageBuffer.hpp"
#include "BlockBuffer.hpp"

#include <unistd.h>

using namespace std;

namespace SocketServer
{

MessageBuffer&
MessageBuffer::Singleton()
{
  static MessageBuffer theObject;
  return theObject;
}

MessageBuffer::MessageBuffer()
  : fdRead_(-1), fdWrite_(-1)
{
  int pipefd[2];
  if (::pipe(pipefd) == -1) {
    logger << logger.fatal << "MessageBuffer failed to open pipe: " << strerror(errno) << endlog;
  }
  fdRead_ = pipefd[0];
  fdWrite_ = pipefd[1];
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
    queueOut_.push_back(pMsg);
    outMsgBySocketID_[socketID].push_back(--queueOut_.end());
    logger << logger.test << "socket=" << socketID << " queue message " << pMsg->getName() << endlog;
  }
  return true;
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
      pMsg = *itor;
      logger << logger.test << "socket=" << socketID << " send message " << pMsg->getName() << endlog;
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
  queueIn_.push_back(pMsg);
  inMsgBySocketID_[socketID].push_back(--queueIn_.end());
  logger << logger.test << "socket=" << socketID << " extracted " << pMsg->getName() << " (" << numBytesExtracted << " bytes)" << endlog;
  queueMessageToSend(pMsg, socketID);
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
