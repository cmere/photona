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
    virtual void print_(std::ostream&) const;
    virtual void parse_(std::istream&);
    virtual bool parse_(BlockBuffer&, unsigned int& offset);

  private:
    std::string content_;
};

}

#endif
