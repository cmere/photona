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

    void setData(const std::string& data) { data_ = data; }
    void setMessageType(int testType) { type_ = testType; }
    void setSendTruncatedData() { isTestTruncatedData_ = true; }

  protected:
    virtual void print_(std::ostream&) const;
    virtual void parse_(std::istream&);

  private:
    std::string data_;
    bool isTestTruncatedData_ = false;
};

}

#endif
