#include "include/first.hpp"
#include "MessageBase.hpp"

using namespace std;

namespace SocketServer
{

pair<unique_ptr<char>, unsigned int> 
MessageBase::toBytes() const
{
  return make_pair(unique_ptr<char>(), 0);
}

}
