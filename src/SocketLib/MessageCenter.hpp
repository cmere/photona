#ifndef SOCKETLIB_MESSAGECENTER_HPP
#define SOCKETLIB_MESSAGECENTER_HPP

#include <memory>
#include "ISelectable.hpp"
#include "MessageBuffer.hpp"

namespace SocketLib
{

/**
 * Control center for messages. Incoming messages are processed here.
 */
class MessageCenter : public ISelectable
{
  public:
    static MessageCenter& Object();
    static std::shared_ptr<MessageCenter> SharedPtr();

    virtual int fd() const;
    virtual void close() { }
    virtual bool isValid() const { return true; }

    virtual int handleSelectReadable();
    virtual int handleSelectWritable() { return 0; }

    virtual const SocketID& getSocketID() const { return MessageCenterPipeID; };
    virtual bool hasBytesToSend() const { return false; } 

  private:
    MessageCenter();
    MessageCenter(const MessageCenter&) = delete;
    MessageCenter& operator=(const MessageCenter&) = delete;

  private:
};

}

#endif
