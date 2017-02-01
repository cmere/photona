#ifndef SOCKETSERVER_MESSAGETEST_HPP
#define SOCKETSERVER_MESSAGETEST_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageTest : public MessageBase
{
  public:
    using PeerID = std::string;

    MessageTest(const PeerID& src, const PeerID& dest) : MessageBase(src, dest) { }

    void setBody(const std::string& body) { body_ = body; }

    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

  private:
    std::string body_;
};

}

#endif
