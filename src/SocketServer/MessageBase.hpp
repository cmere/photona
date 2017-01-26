#ifndef SOCKETSERVER_MESSAGEBASE_HPP
#define SOCKETSERVER_MESSAGEBASE_HPP

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
    std::pair<std::unique_ptr<char>, unsigned int> toBytes() const;

    /// to where (ip address:port)
    class To {
      public:
        To(const std::string& dest) : dest_(dest) { }
        std::string str() const { return dest_; }
     
      private:
        std::string dest_;
    };

    /// from where (ipaddress::port)
    class From {
      public:
        From(const std::string& src) : src_(src) { }
        std::string str() const { return src_; }
     
      private:
        std::string src_;
    };

  private:
    To    dest_;
    From  src_; 
};

inline bool operator<(const MessageBase::To& a, const MessageBase::To& b) { return a < b; }
inline bool operator<(const MessageBase::From& a, const MessageBase::From& b) { return a < b; }

}

#endif
