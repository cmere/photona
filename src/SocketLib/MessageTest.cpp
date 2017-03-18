#include "include/first.hpp"
#include "MessageTest.hpp"
#include "BlockBuffer.hpp"

using namespace std;

namespace SocketServer {

void
MessageTest::setData(const char* data, unsigned int length)
{
  data_.reset(new char[length]);
  memcpy(data_.get(), data, length);
}

unsigned int
MessageTest::print_(BlockBuffer& buffer, unsigned int& offset) const
{
  unsigned int count = 0;
  count += MessageBase::print_(buffer, offset);

  // send "ab|c" as data.
  if (isTestTruncatedData_) {
    count += printT_(buffer, string("ab|c"), "data", offset);
  }
  else {
    logger << "MessageTest build string " << endlog;
    string s(data_.get(), 100*1024*1024);
    logger << "MessageTest build string done" << endlog;
    count += printT_(buffer, s, "data", offset);
  }
  return count;
}

bool
MessageTest::parse_(BlockBuffer& buffer, unsigned int& offset)
{
  if (!MessageBase::parse_(buffer, offset)) {
    return false;
  }

  unsigned int datalen = peekDataFieldLength_(buffer, offset);
  if (datalen == 0) {
    return false;
  }
  data_.reset(new char[datalen]);
  if (!parseT_(buffer, data_.get()[0], "data", offset)) {
    return false;
  }
  return true;
}

}
