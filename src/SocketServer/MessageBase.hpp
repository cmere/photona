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
    using PeerID = std::string;

    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

  private:
    PeerID src_; 
    PeerID dest_;
};

inline bool operator<(const MessageBase::PeerID& a, const MessageBase::PeerID& b) { return a < b; }

std::ostream& operator<<(std::ostream& os, const MessageBase&);

}

#endif
