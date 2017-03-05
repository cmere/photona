#include "include/first.hpp"
#include "MessageEcho.hpp"

using namespace std;

namespace SocketServer {

pair<unique_ptr<char>, unsigned int> 
MessageEcho::toBytes() const
{
  ostringstream oss;
  oss << *this;
  unsigned int len = oss.str().size();

  unique_ptr<char> bytes(new char[len]);
  ::memcpy(bytes.get(), oss.str().c_str(), len);

  return make_pair(move(bytes), len);
}

void
MessageEcho::print_(ostream& os) const
{
  MessageBase::print_(os);
  os << content_;
}

void
MessageEcho::parse_(istream& is)
{
  MessageBase::parse_(is);
  is >> content_;
}

}
