#ifndef SOCKETSERVER_MESSAGEBASE_HPP
#define SOCKETSERVER_MESSAGEBASE_HPP

#include <assert.h>
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
    static std::pair<std::unique_ptr<char>, unsigned int> toBytes(const MessageBase&);

    virtual ~MessageBase() { }

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

    template<class T>
      void printT_(std::ostream& os, const T& t) const
      {
        std::string s = std::to_string(t);
        os << s.size() << '|' << s;
      }

    template<class T>
      static bool parseT_(std::istream& is, T& t)
      {
        logger << "template memeber function MessageBase::parseT_() must be specialized."<< endlog;
        return false;
      }

  private:
    static int parseData_(std::istream& is, std::string& str) {  // may throw exception
      char strlen[10];
      is.get(strlen, 10, '|');
      is.get(); // eat '|'
      int len = std::stoi(strlen);
      if (len <= 0) {
        return len;
      }
      else if (len == 1) {
        char c= is.get();
        str = std::string(&c, 1);
      }
      else {
        std::unique_ptr<char> buf(new char[len+1]);
        is.get(buf.get(), len+1, '\0');
        str = std::string(buf.get());
      }
      return len;
    }

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

template<>
  inline void MessageBase::printT_<std::string>(std::ostream& os, const std::string& t) const
  {
    os << t.size() << '|' << t;
  }

template<>
  inline bool MessageBase::parseT_<std::string>(std::istream& is, std::string& t) 
  {
    try {
      std::string str;
      if (parseData_(is, t) <= 0) {
        return false;
      }
      return true;
    }
    catch (...) {
      return false;
    }
  }

template<>
  inline bool MessageBase::parseT_<unsigned int>(std::istream& is, unsigned int& t)
  {
    try {  // stoi stoul may throw exception
      std::string str;
      if (parseData_(is, str) <= 0) {
        return false;
      }
      t = std::stoul(str);
      return true;
    }
    catch (...) {
      return false;
    }
  }
}

#endif
