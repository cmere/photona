#include "include/first.hpp"
#include "MessageTest.hpp"

using namespace std;

namespace SocketServer {

void
MessageTest::print_(ostream& os) const
{
  MessageBase::print_(os);
  // send "ab|c" as data.
  if (isTestTruncatedData_) {
    os << "ab|c"; 
  }
  else {
    printT_(os, data_);
  }
}

void
MessageTest::parse_(istream& is)
{
  MessageBase::parse_(is);
  parseT_(is, data_, "data");
}

}
