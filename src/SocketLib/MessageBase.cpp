#include "include/first.hpp"
#include "MessageBase.hpp"
#include "BlockBuffer.hpp"
#include "MessageEcho.hpp"
#include "MessageTest.hpp"

#include <exception>

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
      logger << "error: failed to parse message length. clear buffer. " << string(buffer.getDataPtr(), buffer.getContinuousDataSize()) << endlog;
      buffer.clearAll();
      return -1;
    }
    unsigned int msgBodyLen = stoul(strMsgBodyLen);

    // to calculate total length, e.g. "2|12abcdefghijkl"  message body length is 12, total length is 16.
    msglen = to_string(strMsgBodyLen.size()).size() + 1 + strMsgBodyLen.size() + msgBodyLen;
    if (buflen < msglen) {
      logger << "partial message " << buflen << " v.s. " << msglen << endlog;
      return 0;  // partial message.
    }
  }
  catch (...) {
    logger << "error: failed to get message length. clear buffer. " << string(buffer.getDataPtr(), buffer.getContinuousDataSize()) << endlog;
    buffer.clearAll();
    return -1;
  }

  int type = 0;
  // get message type
  try {
    string strType;
    unsigned int typeFieldBytes = MessageBase::getDataField_(buffer, strType, offset);
    if (typeFieldBytes <= 0) {
      logger << "error: failed to parse message type. " << string(buffer.getDataPtr(), buffer.getContinuousDataSize()) << endlog;
      buffer.resizePop(msglen);
      return -1;
    };
    type = stoi(strType);
    offset -= typeFieldBytes;  // put back
  }
  catch (...) {
    logger << "error: failed to get message type. clear message. " << string(buffer.getDataPtr(), buffer.getContinuousDataSize()) << endlog;
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
    logger << "error: unknown message type " << type << endlog;
    buffer.resizePop(msglen);
    return -1;
  }

  if (!pMsg->parse_(buffer, offset)) {
    logger << "error: parse error. clear message. " << string(buffer.getDataPtr(), buffer.getContinuousDataSize()) << endlog;
    pMsg.reset();
    buffer.resizePop(msglen);
    return -1;
  }

  buffer.resizePop(msglen);
  return msglen;
}

unsigned int
MessageBase::toBytes(BlockBuffer& buffer, const MessageBase& msg)
{
  try {
    unsigned int offset = 0;
    unsigned int count = 0;
    // put msg length at first, then update this length after real length is known.
    string msglenStr = "10|0123456789";
    count = buffer.append(msglenStr.c_str(), msglenStr.size(), offset);
    offset += count;
    unsigned int msglen = msg.print_(buffer, offset);
    if (msglen <= 0) {
      logger << "failed to write message to buffer. " << msg.getName() << endlog;
      return 0;
    }
    count += msglen;

    // replace message length by real one.
    msglenStr = to_string(msglen);   // "6789"
    msglenStr = string(10 - msglenStr.size(), '0') + msglenStr;   // padding leading zero "0000006789"
    msglenStr = "10|" + msglenStr;  // "10|0000006789"
    buffer.append(msglenStr.c_str(), msglenStr.size(), 0);  // replace

    buffer.resizePush(count);
    return count;
  }
  catch (...) {
    return 0;
  }
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

bool
MessageBase::parse_(BlockBuffer& buffer, unsigned int& offset)
{
  return parseT_(buffer, type_, "type", offset);
}

unsigned int
MessageBase::print_(BlockBuffer& buffer, unsigned int& offset) const
{
  return printT_(buffer, type_, "type", offset);
}

template<>
bool MessageBase::parseT_<char>(BlockBuffer& buffer, char& t, const std::string& fieldName, unsigned int& offset)
{
  try {
    char* dest = &t;
    // "length|data",  e.g. "2|127|hello 0": two fields: "12" and "hello 0"
    std::string strlen;
    if (buffer.getline(strlen, '|', offset)) {
      unsigned int len = std::stoul(strlen); // throw exceptions
      if (buffer.getdata(dest, len, offset + strlen.size() + 1) == len) {
        unsigned int fieldBytes = strlen.size() + 1 + len; // return total bytes for this field.
        offset += fieldBytes;
        return true;
      }
      else {
        logger << getName() << " failed to parse field data " << fieldName << endlog;
        return false;
      }
    }
    else {
      logger << getName() << " failed to parse field length " << fieldName << endlog;
      return false;
    }
  }
  catch (...) {
    logger << getName() << " failed to parse field " << fieldName << endlog;
    return false;
  }
}

template<>
unsigned int MessageBase::printT_<std::string>(BlockBuffer& buffer, const std::string& t, const string& fieldName, unsigned int& offset)
{
  try{
    auto lenstr = std::to_string(t.size()) + '|';
    unsigned int count = 0;
    if ((count = buffer.append(lenstr.c_str(), lenstr.size(), offset)) != lenstr.size()) {
      throw exception();
    }
    offset += count;

    if ((count = buffer.append(t.c_str(), t.size(), offset)) != t.size()) {
      throw exception();
    }
    offset += count;

    return lenstr.size() + t.size();
  }
  catch (...) {
    logger << "failed to print field " << fieldName << endlog;
    throw;
  }
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

} // namespace SocketServer

