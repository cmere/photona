#include "include/first.hpp"
#include "MessageTest.hpp"

using namespace std;

namespace SocketServer {

pair<unique_ptr<char>, unsigned int> 
MessageTest::toBytes() const
{
  string baseBytes = MessageBase::toString_();
  unsigned int len = baseBytes.size() + body_.size();

  unique_ptr<char> bytes(new char[len]);
  ::memcpy(bytes.get(), baseBytes.c_str(), baseBytes.size());
  ::memcpy(bytes.get() + baseBytes.size(), body_.c_str(), body_.size());

  return make_pair(move(bytes), len);
}

}
