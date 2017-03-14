#ifndef SOCKETSERVER_MESSAGETEST_HPP
#define SOCKETSERVER_MESSAGETEST_HPP

#include "MessageBase.hpp"

namespace SocketServer
{

class MessageTest : public MessageBase
{
  public:
    MessageTest() : MessageBase(MessageBase::TTest) { }

    virtual std::string getName() const { return "MessageTest"; }

    void setData(const char* data, unsigned int length);
    void setMessageType(int testType) { type_ = testType; }
    void setSendTruncatedData() { isTestTruncatedData_ = true; }

  protected:
    virtual bool parse_(BlockBuffer&, unsigned int& offset);
    virtual unsigned int print_(BlockBuffer&, unsigned int& offset) const;

  private:
    std::unique_ptr<char> data_;
    bool isTestTruncatedData_ = false;
};

}

#endif
