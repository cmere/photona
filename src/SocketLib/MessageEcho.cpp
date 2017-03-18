#include "include/first.hpp"
#include "MessageEcho.hpp"
#include "BlockBuffer.hpp"

using namespace std;
using namespace Util;

namespace SocketLib {

unsigned int
MessageEcho::print_(BlockBuffer& buffer, unsigned int& offset) const
{
  unsigned int count = 0;
  count += MessageBase::print_(buffer, offset);
  count += printT_(buffer, content_, "content", offset);
  return count;
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
