#ifndef SOCKETLIB_MESSAGECENTER_HPP
#define SOCKETLIB_MESSAGECENTER_HPP

#include <memory>
#include <set>
#include "ISelectable.hpp"
#include "MessageBase.hpp"
#include "MessageBuffer.hpp"
#include "SocketID.hpp"

namespace SocketLib
{

/**
 * Control center for messages. Incoming messages are processed here.
 */
class MessageCenter : public ISelectable
{
  public:
    static std::shared_ptr<MessageCenter> GetSharedPtr();
    static void SetSharedPtr(std::shared_ptr<MessageCenter>);

    // from ISelectable
    virtual int fd() const;
    virtual void close() { }
    virtual bool isValid() const { return true; }

    virtual int handleSelectReadable();
    virtual int handleSelectWritable() { return 0; }

    virtual const SocketID& getSocketID() const { return MessageCenterPipeID; };
    virtual bool hasBytesToSend() const { return false; }

    // MessageCenter virtual functions
    virtual std::set<SocketID> getFinishedSocketIDs() { return std::set<SocketID>(); }
    virtual void clearFinishedSocketIDs() { }

  protected:
    MessageCenter();
    MessageCenter(const MessageCenter&) = delete;
    MessageCenter& operator=(const MessageCenter&) = delete;

  private:
    static std::shared_ptr<MessageCenter> pObject_;
};

}

#endif
