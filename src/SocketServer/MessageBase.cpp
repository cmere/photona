#include "include/first.hpp"
#include "MessageBase.hpp"
#include "BlockBuffer.hpp"
#include "MessageEcho.hpp"
#include "MessageTest.hpp"

using namespace std;

namespace SocketServer
{

int
MessageBase::fromBytes(BlockBuffer& buffer, shared_ptr<MessageBase>& pMsg)
{
  unsigned int buflen = buffer.getTotalDataSize();
  if (buflen == 0) {
    return 0;
  }

  unsigned int msglen = 0;
  unsigned int offset = 0;
  // see if buffer hold a whole message (length)
  try {
    string strMsgBodyLen;
    unsigned int fieldNumBytes = MessageBase::getDataField_(buffer, strMsgBodyLen, offset);
    if (fieldNumBytes == 0) {
      logger << "error: failed to parse message length. clear buffer." << endlog;
      buffer.clearAll();
      return -1;
    }
    unsigned int msgBodyLen = stoul(strMsgBodyLen);

    // to calculate total length, e.g. "2|12abcdefghijkl"  message body length is 12, total length is 16.
    msglen = to_string(strMsgBodyLen.size()).size() + 1 + strMsgBodyLen.size() + msgBodyLen;
    if (buflen < msglen) {
      logger << logger.test << "partial message " << buflen << " v.s. " << msglen << endlog;
      return 0;  // partial message.
    }
  }
  catch (...) {
    logger << "error: failed to get message length. clear buffer." << endlog;
    buffer.clearAll();
    return -1;
  }

  int type = 0;
  // get message type
  try {
    string strType;
    unsigned int typeFieldBytes = MessageBase::getDataField_(buffer, strType, offset);
    if (typeFieldBytes <= 0) {
      logger << "error: failed to parse message type." << endlog;
      buffer.resizePop(msglen);
      return -1;
    };
    type = stoi(strType);
    offset -= typeFieldBytes;  // put back
  }
  catch (...) {
    logger << "error: failed to get message type. clear message." << endlog;
    buffer.resizePop(msglen);
    return -1;
  }

  // create Message
  if (type == TEcho) {
    pMsg.reset(new MessageEcho());
  }
  else if (type == TTest) {
    pMsg.reset(new MessageTest());
  }
  else {
    logger << logger.test << "error: unknown message type " << type << endlog;
    buffer.resizePop(msglen);
    return -1;
  }

  if (!pMsg->parse_(buffer, offset)) {
    logger << "error: parse error. clear message. " << endlog;
    pMsg.reset();
    buffer.resizePop(msglen);
    return -1;
  }

  buffer.resizePop(msglen);
  return msglen;
}

std::pair<unique_ptr<char>, unsigned int> 
MessageBase::toBytes(const MessageBase& msg)
{
  logger << "1" << endlog;
  ostringstream oss;
  logger << "2" << endlog;
  oss << msg;
  logger << "3" << endlog;
  unsigned int len = oss.str().size();
  logger << "4" << endlog;

  // put the total length at the begining. e.g. 10|0123456789
  ostringstream ossHead;
  logger << "5" << endlog;
  printT_(ossHead, len);
  logger << "6" << endlog;
  unsigned int headLen = ossHead.str().size();
  logger << "7" << endlog;
  unique_ptr<char> bytes(new char[headLen + len]);
  logger << "8" << endlog;
  ::memcpy(bytes.get(), ossHead.str().c_str(), headLen);
  logger << "9" << endlog;
  char* a = bytes.get() + headLen;
  logger << "9" << endlog;
  const string& tmp = oss.str();
  logger << "10" << endlog;
  const char* b = tmp.c_str();
  logger << "11" << endlog;
  ::memcpy(a, b, len);
  logger << "12" << endlog;

  return make_pair(move(bytes), headLen + len);
}

unsigned int
MessageBase::peekDataFieldLength_(BlockBuffer& buffer, unsigned int& offset)
{
  try {
    string strlen;
    if (buffer.getline(strlen, '|', offset)) {
      unsigned int len = std::stoul(strlen); // throw exceptions
      return len;
    }
  }
  catch (...) {
    return 0;
  }
  return 0;
}

unsigned int 
MessageBase::getDataField_(BlockBuffer& buffer, std::string& strdata, unsigned int& offset)
{
  // "length|data",  e.g. "2|127|hello 0": two fields: "12" and "hello 0"
  string strlen;
  if (buffer.getline(strlen, '|', offset)) {
    unsigned int len = std::stoul(strlen); // throw exceptions
    if (buffer.getdata(strdata, len, offset + strlen.size() + 1) == len) {
      unsigned int fieldBytes = strlen.size() + 1 + len; // return total bytes for this field.
      offset += fieldBytes;
      return fieldBytes;
    }
  }
  return 0;
}

int 
MessageBase::parseData_(std::istream& is, std::string& str)
{
  // e.g. "2|127|hello 0": two fields: "12" and "hello 0"
  char strlen[10];
  is.get(strlen, 10, '|');
  is.get(); // eat '|'
  int len = std::stoi(strlen);
  if (len <= 0) {
    return len;
  }
  else {
    std::unique_ptr<char> buf(new char[len]);
    is.read(buf.get(), len);
    str = std::string(buf.get(), len);
  }
  return len;
}

bool
MessageBase::parse_(BlockBuffer& buffer, unsigned int& offset)
{
  return parseT_(buffer, type_, "type", offset);
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
