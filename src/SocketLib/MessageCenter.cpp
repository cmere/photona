#include "include/first.hpp"
#include "MessageCenter.hpp"

using namespace std;
using namespace Util;

namespace SocketLib
{

shared_ptr<MessageCenter>
MessageCenter::SharedPtr()
{
  static shared_ptr<MessageCenter> pObject;
  if (!pObject) {
    pObject.reset(new MessageCenter);
  }
  return pObject;
}

MessageCenter&
MessageCenter::Object()
{
  return *SharedPtr();
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
  auto pMsg = MessageBuffer::Singleton().popInMessage();
  if (!pMsg) {
    logger << "MessageCenter failed to pop message from MessageBuffer." << endlog;
    return 0;
  }

  logger << "discard message " << pMsg->getName() << endlog;
  return 0;
}

}  // namespace SocketLib
