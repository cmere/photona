#ifndef SOCKETSERVER_MESSAGEBASE_HPP
#define SOCKETSERVER_MESSAGEBASE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace SocketServer
{

/**
 * Base class for message classes.
 */
class MessageBase
{
  public:
    static std::unique_ptr<MessageBase> fromBytes(const char* bytes, unsigned int length);

    virtual ~MessageBase() { }

    virtual std::pair<std::unique_ptr<char>, unsigned int> toBytes() const = 0;

    friend std::istream& operator>>(std::istream&,       MessageBase&);
    friend std::ostream& operator<<(std::ostream&, const MessageBase&);

  protected:
    enum { 
      TEcho = 1,
    };

    MessageBase(unsigned int type) : type_(type)
    { }

    virtual void parse_(std::istream&);
    virtual void print_(std::ostream&) const;

  private:
    unsigned int type_;
};

//inline bool operator<(const MessageBase::PeerID& a, const MessageBase::PeerID& b) { return a < b; }

inline std::istream& operator>>(std::istream& is, MessageBase& msg)
{
  msg.parse_(is);
  return is;
}

inline std::ostream& operator<<(std::ostream& os, const MessageBase& msg)
{
  msg.print_(os);
  return os;
}

}

#endif
