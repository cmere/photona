#ifndef SOCKETLIB_MESSAGEBASE_HPP
#define SOCKETLIB_MESSAGEBASE_HPP

#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace SocketLib
{

class BlockBuffer;

/**
 * Base class for message classes.
 */
class MessageBase
{
  public:
    static int fromBytes(BlockBuffer&, std::shared_ptr<MessageBase>&);
    static unsigned int toBytes(BlockBuffer&, const MessageBase&);

    enum { 
      TEcho = 1,
      TTest, 
    };

    virtual ~MessageBase() { }
    int getType() const { return type_; }
    virtual std::string getName() const { return "MessageBase"; }

  private:
    template<class T> 
      class ParseTrait
      {
        public:
          static T convert(const std::string&);
      };

  protected:
    MessageBase(int type) : type_(type)
    { }

    virtual bool parse_(BlockBuffer&, unsigned int& offset);
    virtual unsigned int print_(BlockBuffer&, unsigned int& offset) const;

    template<class T>
      static unsigned int printT_(BlockBuffer& buffer, const T& t, const std::string& fieldName, unsigned int& offset)
      {
        std::string s = std::to_string(t);
        return printT_(buffer, s, fieldName, offset);
      }

    template<class T>
      bool parseT_(BlockBuffer& buffer, T& t, const std::string& fieldName, unsigned int& offset)
      {
        try {  // stoi stoul may throw exception
          std::string str;
          unsigned int fieldBytes = getDataField_(buffer, str, offset);
          if (fieldBytes <= 0) {
            Util::logger << getName() << " failed to parse field " << fieldName << Util::endlog;
            return false;
          }
          t = ParseTrait<T>::convert(str);
          return true;
        }
        catch (...) {
          Util::logger << getName() << " failed to parse field " << fieldName << Util::endlog;
          return false;
        }
      }

  protected:
    static unsigned int peekDataFieldLength_(BlockBuffer&, unsigned int& offset); // throw exceptions

  private:
    static unsigned int getDataField_(BlockBuffer&, std::string&, unsigned int& offset); // throw exceptions

  private:
    int type_;
    friend class MessageTest;
};

///////////////////////////////////////////////////////////////////////////////////////
// specialization 

template<>
  bool MessageBase::parseT_<char>(BlockBuffer& buffer, char& t, const std::string& fieldName, unsigned int& offset);

template<>
  unsigned int MessageBase::printT_<std::string>(BlockBuffer& buffer, const std::string& t, const std::string& fieldName, unsigned int& offset);

template<> 
  class MessageBase::ParseTrait<unsigned int> {
    public:
      static unsigned int convert(std::string& str) { return std::stoul(str); }
  };

template<> 
  class MessageBase::ParseTrait<int> {
    public:
      static int convert(std::string& str) { return std::stoi(str); }
  };

template<> 
  class MessageBase::ParseTrait<std::string> {
    public:
      static std::string convert(std::string& str) { return str; }
  };

// specialization
///////////////////////////////////////////////////////////////////////////////////////


}  // namespace SocketLib

#endif
