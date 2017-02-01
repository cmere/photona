#include "include/first.hpp"
#include "MessageBase.hpp"

using namespace std;

namespace SocketServer
{

ostream& operator<<(ostream& os, const MessageBase& msg)
{
  os << " MessageBase " << endl;
  return os;
}

}
