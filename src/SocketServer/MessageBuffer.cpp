#include "include/first.hpp"
#include "MessageBuffer.hpp"

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
{ }

bool
MessageBuffer::hasMessageToSend(const MessageBase::ClientID& to) const
{
  return outMsgByClientID_.count(to) > 0;
}

bool 
MessageBuffer::queueMessageToSend(const std::shared_ptr<MessageBase>& pMsg)
{
  if (pMsg) {
    queueOut_.push_back(pMsg);
    outMsgByClientID_[pMsg->getClientID()].push_back(--queueOut_.end());
  }
  return true;
}

shared_ptr<MessageBase> 
MessageBuffer::popMessageToSend(const MessageBase::ClientID& to)
{
  shared_ptr<MessageBase> pMsg;
  auto found = outMsgByClientID_.find(to);
  if (found != outMsgByClientID_.end()) {
    auto& itorList = found->second;
    if (!itorList.empty()) {
      auto itor = *itorList.begin();
      pMsg = *itor;
      queueOut_.erase(itor);
      itorList.erase(itorList.begin());
      if (itorList.empty()) {
        outMsgByClientID_.erase(found);
      }
    }
  }
  return pMsg;
}

unsigned int
MessageBuffer::extractMessageFromBytes(const char* bytes, unsigned int length)
{
  logger << "extract " << length << " bytes: " << string(bytes, length) << endlog;
  return length;
}

bool 
MessageBuffer::canReadMore(const MessageBase::ClientID& src) const
{
  return true;
}

unsigned int
MessageBuffer::removeSocketMessages(const MessageBase::ClientID&)
{
  return 0;
}

}
