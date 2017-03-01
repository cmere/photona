#ifndef SOCKETSERVER_MESSAGEECHO_HPP
#define SOCKETSERVER_MESSAGEECHO_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageEcho : public MessageBase
{
  public:
    MessageEcho() : MessageBase() { }

    void setContent(const std::string& content) { content_ = content; }

    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

  private:
    std::string content_;
};

}

#endif
