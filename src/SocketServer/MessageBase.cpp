#include "include/first.hpp"
#include "MessageBase.hpp"
#include "MessageEcho.hpp"

using namespace std;

namespace SocketServer
{

std::unique_ptr<MessageBase> 
MessageBase::fromBytes(const char* bytes, unsigned int length)
{
  unique_ptr<MessageBase> pMsg;

  if (length > 0) {
    istringstream iss(string(bytes, length));
    // peek message type
    unsigned int type;
    iss >> type;
    iss.seekg(ios_base::beg);
    if (iss) {
      if (type == TEcho) {
        pMsg.reset(new MessageEcho());
        iss >> *pMsg;
      }
    }

    if (!iss) {
      pMsg = nullptr;
      logger << "error: failed to create message from bytes. type=" << type << endlog;
    }
  }

  return move(pMsg);
}

void
MessageBase::parse_(istream& is)
{
  is >> type_;
}

void
MessageBase::print_(ostream& os) const
{
  os << type_;
}




}
