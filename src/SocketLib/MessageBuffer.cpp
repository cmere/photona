#include "include/first.hpp"
#include "MessageBuffer.hpp"
#include "BlockBuffer.hpp"

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
MessageBuffer::popMessage_(const SocketID& socketID, MsgBySocketID& msgBySocketID)
{
  shared_ptr<MessageBase> pMsg;
  auto found = msgBySocketID.find(socketID);
  if (found != msgBySocketID.end()) {
    auto& itorList = found->second;
    if (!itorList.empty()) {
      auto itor = *itorList.begin();
      pMsg = *itor;
      queueOut_.erase(itor);
      itorList.erase(itorList.begin());
      if (itorList.empty()) {
        msgBySocketID.erase(found);
      }
    }
  }
  return pMsg;
}

shared_ptr<MessageBase> 
MessageBuffer::popInMessage(const SocketID& socketID)
{
  return popMessage_(socketID, inMsgBySocketID_);
}

shared_ptr<MessageBase> 
MessageBuffer::popOutMessage(const SocketID& socketID)
{
  return popMessage_(socketID, outMsgBySocketID_);
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
  pMsg = popInMessage(socketID);
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
