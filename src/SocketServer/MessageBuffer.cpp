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

shared_ptr<MessageBase> 
MessageBuffer::popMessage(const MessageBase::PeerID& to)
{
  return shared_ptr<MessageBase>();
}

unsigned int
MessageBuffer::extractMessageFromBytes(const char*, unsigned int length)
{
  return 0;
}

bool 
MessageBuffer::needReadMore(const MessageBase::PeerID& src) const
{
  return true;
}

unsigned int
MessageBuffer::removeSocketMessages(const MessageBase::PeerID&)
{
  return 0;
}

}
