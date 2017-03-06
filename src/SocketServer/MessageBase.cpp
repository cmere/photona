#include "include/first.hpp"
#include "MessageBase.hpp"
#include "MessageEcho.hpp"

using namespace std;

namespace SocketServer
{

std::pair<std::unique_ptr<MessageBase>, unsigned int>
MessageBase::fromBytes(const char* bytes, unsigned int length)
{
  unique_ptr<MessageBase> pMsg;
  unsigned int totalLength = 0;

  if (length > 0) {
    istringstream iss(string(bytes, length));

    // peek message length and type
    unsigned int type = 0;
    try {
      string strMsgLen;
      ;
      if (MessageBase::parseData_(iss, strMsgLen) <= 0) {
        logger << "error: failed to parse message length." << endlog;
        return make_pair(unique_ptr<MessageBase>(nullptr), length);
      }

      // to calculate totalLength, e.g. "2|12abcdefghijkl"  message length is 12, total length is 16.
      totalLength = to_string(strMsgLen.size()).size() + 1 + strMsgLen.size() + stoul(strMsgLen);  
      if (length < totalLength) {
        logger << logger.test << "partial message " << length << " : " << totalLength<< endlog;
        return make_pair(unique_ptr<MessageBase>(nullptr), 0);
      }

      string strType;
      if (MessageBase::parseData_(iss, strType) <= 0) {
        logger << "error: failed to parse message type." << endlog;
        return make_pair(unique_ptr<MessageBase>(nullptr), length);
      };
      iss.seekg(ios_base::beg);
      type = stoul(strType);
    }
    catch (...) {
      logger << "error: failed to parse message type." << endlog;
      return make_pair(unique_ptr<MessageBase>(nullptr), length);
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

  return make_pair(move(pMsg), totalLength);
}

std::pair<unique_ptr<char>, unsigned int> 
MessageBase::toBytes(const MessageBase& msg)
{
  ostringstream oss;
  oss << msg;
  unsigned int len = oss.str().size();

  // put the total length at the begining. e.g. 10|0123456789
  ostringstream ossHead;
  printT_(ossHead, len);
  unsigned int headLen = ossHead.str().size();
  unique_ptr<char> bytes(new char[headLen + len]);
  ::memcpy(bytes.get(), ossHead.str().c_str(), headLen);
  ::memcpy(bytes.get() + headLen, oss.str().c_str(), len);

  return make_pair(move(bytes), headLen + len);
}

int 
MessageBase::parseData_(std::istream& is, std::string& str)
{
  char strlen[10];
  is.get(strlen, 10, '|');
  is.get(); // eat '|'
  int len = std::stoi(strlen);
  if (len <= 0) {
    return len;
  }
  else if (len == 1) {
    char c= is.get();
    str = std::string(&c, 1);
  }
  else {
    std::unique_ptr<char> buf(new char[len+1]);
    is.get(buf.get(), len+1, '\0');
    str = std::string(buf.get());
  }
  return len;
}

void
MessageBase::parse_(istream& is)
{
  unsigned int numBytes;
  parseT_(is, numBytes, "Field0");
  parseT_(is, type_, "type");
}

void
MessageBase::print_(ostream& os) const
{
  printT_(os, type_);
}

}
