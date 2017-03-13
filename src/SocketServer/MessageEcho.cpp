#include "include/first.hpp"
#include "MessageEcho.hpp"
#include "BlockBuffer.hpp"

using namespace std;

namespace SocketServer {

void
MessageEcho::print_(ostream& os) const
{
  MessageBase::print_(os);
  printT_(os, content_);
}

void
MessageEcho::parse_(istream& is)
{
  MessageBase::parse_(is);
  parseT_(is, content_, "content");
}

bool
MessageEcho::parse_(BlockBuffer& buffer, unsigned int& offset)
{
  if (!MessageBase::parse_(buffer, offset)) {
    return false;
  }

  if (!parseT_(buffer, content_, "content", offset)) {
    return false;
  }
  return true;
}

}
