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

void
MessageTest::print_(ostream& os) const
{
  MessageBase::print_(os);
  // send "ab|c" as data.
  if (isTestTruncatedData_) {
    os << "ab|c"; 
  }
  else {
    printT_(os, string(data_.get(), 100*1024*1024));
  }
}

void
MessageTest::parse_(istream& is)
{
  MessageBase::parse_(is);
 // parseT_(is, data_, "data");
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
