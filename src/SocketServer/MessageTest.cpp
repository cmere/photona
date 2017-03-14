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
  unsigned int bytesPrinted = 0;
  unsigned int count = 0;
  if ((count = MessageBase::print_(buffer, offset)) == 0) {
    return 0;
  }
  else {
    bytesPrinted += count;
  }

  // send "ab|c" as data.
  if (isTestTruncatedData_) {
    if ((count = printT_(buffer, string("ab|c"), offset)) == 0) {
      return 0;
    }
    else {
      bytesPrinted += count;
    }
  }
  else {
    logger << "MessageTest build string " << endlog;
    string s(data_.get(), 100*1024*1024);
    logger << "MessageTest build string done" << endlog;
    if ((count = printT_(buffer, s, offset)) == 0) {
      return 0;
    }
    else {
      bytesPrinted += count;
    }
  }
  return bytesPrinted;
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
