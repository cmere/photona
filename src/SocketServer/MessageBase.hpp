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
    using ClientID = std::string;

    MessageBase(const ClientID& clientID) 
      : clientID_(clientID) { }
    virtual ~MessageBase() { }

    virtual std::pair<std::unique_ptr<char>, unsigned int> toBytes() const = 0;
    const ClientID& getClientID() const { return clientID_; }

  protected:
    std::string toString_() const { return clientID_; }

  private:
    ClientID clientID_; 
};

inline bool operator<(const MessageBase::ClientID& a, const MessageBase::ClientID& b) { return a < b; }

std::ostream& operator<<(std::ostream& os, const MessageBase&);

}

#endif
