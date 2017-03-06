#include "include/first.hpp"
#include "MessageEcho.hpp"

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

}
