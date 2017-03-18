#include "include/first.hpp"
#include "MessageCenter.hpp"

using namespace std;
using namespace Util;

namespace SocketLib
{

shared_ptr<MessageCenter> MessageCenter::pObject_;

shared_ptr<MessageCenter>
MessageCenter::GetSharedPtr()
{
  if (!pObject_) {
    pObject_.reset(new MessageCenter);  // default message center
  }
  return pObject_;
}

void 
MessageCenter::SetSharedPtr(std::shared_ptr<MessageCenter> pObj)
{
  pObject_ = pObj;
}

MessageCenter::MessageCenter()
{ 
}

int
MessageCenter::fd() const
{
  return MessageBuffer::Singleton().getInMessagePipeReadFD();
}

int
MessageCenter::handleSelectReadable()
{
  char c;
  if (::read(fd(), &c, 1) != 1) {
    logger << "MessageCenter failed to read from pipe." << endlog;
  }
  auto socketID_pMsg = MessageBuffer::Singleton().popFirstMessageInQueue();
  auto socketID = socketID_pMsg.first;
  auto pMsg = socketID_pMsg.second;
  if (!pMsg) {
    logger << "MessageCenter failed to pop message from MessageBuffer." << endlog;
    return 0;
  }

  logger << "discard message " << pMsg->getName() << " received on socketID=" << socketID << endlog;
  return 0;
}

}  // namespace SocketLib
