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
    MessageBase() { }
    virtual ~MessageBase() { }

    virtual std::pair<std::unique_ptr<char>, unsigned int> toBytes() const = 0;

  protected:
    std::string toString_() const { return ""; }

  private:
};

//inline bool operator<(const MessageBase::PeerID& a, const MessageBase::PeerID& b) { return a < b; }

std::ostream& operator<<(std::ostream& os, const MessageBase&);

}

#endif
