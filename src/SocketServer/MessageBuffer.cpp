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
MessageBuffer::hasMessageToSend(MessageBase::To to) const
{
  return outMsgByDest_.count(to) > 0;
}

shared_ptr<MessageBase> 
MessageBuffer::popMessage(MessageBase::To)
{
  return shared_ptr<MessageBase>();
}

unsigned int
MessageBuffer::extractMessageFromBytes(const char*, unsigned int length)
{
  return 0;
}

bool 
MessageBuffer::needReadMore(const MessageBase::From& src) const
{
  return true;
}

}
