#include "include/first.hpp"
#include "MessageEcho.hpp"
#include "BlockBuffer.hpp"

using namespace std;

namespace SocketServer {

unsigned int
MessageEcho::print_(BlockBuffer&, unsigned int& offset) const
{
  return 0;
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
