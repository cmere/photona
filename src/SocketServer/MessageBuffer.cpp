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
MessageBuffer::hasMessageToSend(const MessageBase::PeerID& to) const
{
  return outMsgByDest_.count(to) > 0;
}

bool 
MessageBuffer::queueMessageToSend(const std::shared_ptr<MessageBase>& pMsg)
{
  if (pMsg) {
    queueOut_.push_back(pMsg);
    outMsgByDest_[pMsg->getDest()].push_back(--queueOut_.end());
  }
  return true;
}

shared_ptr<MessageBase> 
MessageBuffer::popMessageToSend(const MessageBase::PeerID& to)
{
  shared_ptr<MessageBase> pMsg;
  auto found = outMsgByDest_.find(to);
  if (found != outMsgByDest_.end()) {
    auto& itorList = found->second;
    if (!itorList.empty()) {
      auto itor = *itorList.begin();
      pMsg = *itor;
      queueOut_.erase(itor);
      itorList.erase(itorList.begin());
      if (itorList.empty()) {
        outMsgByDest_.erase(found);
      }
    }
  }
  return pMsg;
}

unsigned int
MessageBuffer::extractMessageFromBytes(const char* bytes, unsigned int length)
{
  cout << "extract " << length << " bytes: " << string(bytes, length) << endl;
  return length;
}

bool 
MessageBuffer::canReadMore(const MessageBase::PeerID& src) const
{
  return true;
}

unsigned int
MessageBuffer::removeSocketMessages(const MessageBase::PeerID&)
{
  return 0;
}

}
