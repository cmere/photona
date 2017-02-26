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
MessageBuffer::extractMessageFromSocket(const char* bytes, unsigned int length, const SocketID& socketID)
{
  //queueIn_.push_back(make_shared<>(MessageBase));
  logger << logger.debug << "extract " << length << " bytes: " << string(bytes, length) << endlog;
  return length;
}

bool 
MessageBuffer::shouldReadMoreOnSocket(const SocketID& socketID) const
{
  return true;
}

//unsigned int
//MessageBuffer::removeSocketMessages(const SocketID&)
//{
//  return 0;
//}

}
