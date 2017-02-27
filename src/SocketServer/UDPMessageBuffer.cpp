#include "include/first.hpp"
#include "UDPMessageBuffer.hpp"

using namespace std;

namespace SocketServer
{

static unsigned int MaxInUDPMsgs = 10;

UDPMessageBuffer&
UDPMessageBuffer::Singleton()
{
  static UDPMessageBuffer theObject;
  return theObject;
}

UDPMessageBuffer::UDPMessageBuffer()
{ }

bool
UDPMessageBuffer::hasMessageToSend(const SocketID& socketID) const
{
  return outMsgBySocketID_.count(socketID) > 0;
}

bool 
UDPMessageBuffer::queueMessageToSend(const std::shared_ptr<MessageBase>& pMsg,
                                     const SocketID& socketID, 
                                     const string& peerIP,
                                     const unsigned int peerPort)
{
  if (pMsg) {
    queueOut_.push_back(make_tuple(pMsg, peerIP, peerPort));
    outMsgBySocketID_[socketID].push_back(--queueOut_.end());
  }
  return true;
}

tuple<shared_ptr<MessageBase>, string, unsigned int>
UDPMessageBuffer::popMessageToSend(const SocketID& socketID)
{
  tuple<shared_ptr<MessageBase>, string, unsigned int> pMsg;
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
UDPMessageBuffer::extractMessageFromSocket(const char* bytes, 
                                           unsigned int length, 
                                           const SocketID& socketID,
                                           const string& peerIP,
                                           const unsigned int peerPort)
{
  //queueIn_.push_back(make_shared<>(MessageBase));
  logger << "socket " << socketID << " extract " << length << " bytes" << endlog;
  return length;
}

bool 
UDPMessageBuffer::shouldReadMoreOnSocket(const SocketID& socketID) const
{
  const auto& inMsgs = inMsgBySocketID_.find(socketID);
  if (inMsgs != inMsgBySocketID_.end() && inMsgs->second.size() >= MaxInUDPMsgs) {
    logger << logger.debug << "socket " << socketID << " has too many incoming messages, no read. " << " threadhold=" << MaxInUDPMsgs << endlog;
    return false;
  }
  return true;
}

}
