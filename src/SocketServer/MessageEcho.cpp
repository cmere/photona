#include "include/first.hpp"
#include "MessageEcho.hpp"

using namespace std;

namespace SocketServer {

pair<unique_ptr<char>, unsigned int> 
MessageEcho::toBytes() const
{
  string baseBytes = MessageBase::toString_();
  unsigned int len = baseBytes.size() + content_.size();

  unique_ptr<char> bytes(new char[len]);
  ::memcpy(bytes.get(), baseBytes.c_str(), baseBytes.size());
  ::memcpy(bytes.get() + baseBytes.size(), content_.c_str(), content_.size());

  return make_pair(move(bytes), len);
}

}
