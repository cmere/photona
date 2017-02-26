#ifndef SOCKETSERVER_MESSAGETEST_HPP
#define SOCKETSERVER_MESSAGETEST_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageTest : public MessageBase
{
  public:
    MessageTest() : MessageBase() { }

    void setBody(const std::string& body) { body_ = body; }

    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

  private:
    std::string body_;
};

}

#endif
