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

    MessageBase(const PeerID& src, const PeerID& dest) 
      : src_(src), dest_(dest) { }
    virtual ~MessageBase() { }

    virtual std::pair<std::unique_ptr<char>, unsigned int> toBytes() const = 0;
    const PeerID& getSource() const { return src_; }
    const PeerID& getDest() const { return dest_; }

  protected:
    std::string toString_() const { return src_ + " " + dest_; }

  private:
    PeerID src_; 
    PeerID dest_;
};

inline bool operator<(const MessageBase::PeerID& a, const MessageBase::PeerID& b) { return a < b; }

std::ostream& operator<<(std::ostream& os, const MessageBase&);

}

#endif
