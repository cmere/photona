#ifndef SOCKETSERVER_MESSAGEECHO_HPP
#define SOCKETSERVER_MESSAGEECHO_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageEcho : public MessageBase
{
  public:
    MessageEcho() : MessageBase(MessageBase::TEcho) { }

    virtual std::string getName() const { return "MessageEcho"; }

    void setContent(const std::string& content) { content_ = content; }

  protected:
    virtual unsigned int print_(BlockBuffer&, unsigned int& offset) const;
    virtual bool parse_(BlockBuffer&, unsigned int& offset);

  private:
    std::string content_;
};

}

#endif
