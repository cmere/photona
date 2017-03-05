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
    bool isTypeValid = MessageBase::parseT_<unsigned int>(iss, type);
    iss.seekg(ios_base::beg);
    if (!isTypeValid) {
      logger << "error: failed to parse message type." << endlog;
      return unique_ptr<MessageBase>(nullptr);
    }

    if (iss) {
      if (type == TEcho) {
        pMsg.reset(new MessageEcho());
        iss >> *pMsg;
      }
      else {
        pMsg.reset(nullptr);
        logger << "error: unknown message type " << type << endlog;
      }
    }

    if (!iss) {
      pMsg.reset(nullptr);
      logger << "error: failed to create message from bytes. type=" << type << endlog;
    }
  }

  return move(pMsg);
}

std::pair<unique_ptr<char>, unsigned int> 
MessageBase::toBytes(const MessageBase& msg)
{
  ostringstream oss;
  oss << msg;
  unsigned int len = oss.str().size();

  unique_ptr<char> bytes(new char[len]);
  ::memcpy(bytes.get(), oss.str().c_str(), len);

  return make_pair(move(bytes), len);
}

void
MessageBase::parse_(istream& is)
{
  if (!parseT_(is, type_)) {
    logger << "error: fail to parse message type" << endlog;
  }
}

void
MessageBase::print_(ostream& os) const
{
  printT_(os, type_);
}

}
