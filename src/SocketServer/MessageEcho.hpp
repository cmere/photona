#ifndef SOCKETSERVER_MESSAGEECHO_HPP
#define SOCKETSERVER_MESSAGEECHO_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageEcho : public MessageBase
{
  public:
    MessageEcho() : MessageBase(MessageBase::TEcho) { }

    void setContent(const std::string& content) { content_ = content; }

    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

  protected:
    virtual void print_(std::ostream&) const;
    virtual void parse_(std::istream&);

  private:
    std::string content_;
};

}

#endif
