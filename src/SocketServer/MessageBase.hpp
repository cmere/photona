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
    static std::pair<std::unique_ptr<MessageBase>, unsigned int> fromBytes(const char* bytes, unsigned int length);
    static std::pair<std::unique_ptr<char>, unsigned int> toBytes(const MessageBase&);

    virtual ~MessageBase() { }

    virtual std::string getName() const { return "MessageBase"; }

    friend std::istream& operator>>(std::istream&,       MessageBase&);
    friend std::ostream& operator<<(std::ostream&, const MessageBase&);

  private:
    template<class T> 
      class ParseTrait
      {
        public:
          static T convert(const std::string&);
      };

  protected:
    enum { 
      TEcho = 1,
      TTest, 
    };

    MessageBase(int type) : type_(type)
    { }

    virtual void parse_(std::istream&);
    virtual void print_(std::ostream&) const;

    template<class T>
      static void printT_(std::ostream& os, const T& t)
      {
        std::string s = std::to_string(t);
        os << s.size() << '|' << s;
      }

    template<class T>
      bool parseT_(std::istream& is, T& t, const std::string& fieldName)
      {
        try {  // stoi stoul may throw exception
          std::string str;
          if (parseData_(is, str) <= 0) {
            logger << getName() << " failed to parse field " << fieldName << endlog;
            is.setstate(std::ios::failbit);
            return false;
          }
          t = ParseTrait<T>::convert(str);
          return true;
        }
        catch (...) {
          logger << getName() << " failed to parse field " << fieldName << endlog;
          is.setstate(std::ios::failbit);
          return false;
        }
      }

  private:
    static int parseData_(std::istream& is, std::string& str); // throw exception

  private:
    int type_;
    friend class MessageTest;
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

///////////////////////////////////////////////////////////////////////////////////////
// specialization of printT_ 
//
template<>
  inline void MessageBase::printT_<std::string>(std::ostream& os, const std::string& t)
  {
    os << t.size() << '|' << t;
  }
// 
// specialization of printT_ 
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// specialization of ParseTrait template
//
template<> 
  class MessageBase::ParseTrait<unsigned int> {
    public:
      static unsigned int convert(const std::string& str) { return std::stoul(str); }
  };

template<> 
  class MessageBase::ParseTrait<int> {
    public:
      static int convert(const std::string& str) { return std::stoi(str); }
  };

template<> 
  class MessageBase::ParseTrait<std::string> {
    public:
      static std::string convert(const std::string& str) { return str; }
  };
//
// specialization of ParseTrait template
///////////////////////////////////////////////////////////////////////////////////////


}  // namespace SocketServer

#endif
