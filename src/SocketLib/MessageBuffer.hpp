#ifndef SOCKETLIB_MESSAGEBUFFER_HPP
#define SOCKETLIB_MESSAGEBUFFER_HPP

#include <map>
#include <memory>
#include <list>
#include "MessageBase.hpp"
#include "SocketID.hpp"

namespace SocketLib
{

class BlockBuffer;

/**
 * Input/output buffer queue for messages. All in/out socket share this one quque (FIFO).
 *
 * Socket read bytes, send here to construct messages, and save to input queue. 
 * If there are messages in output queue, socket send them.
 */
class MessageBuffer
{
  public:
    static MessageBuffer& Singleton();

    // extract ONE message, put in queue, return number of bytes extracted.
    unsigned int extractMessageFromSocket(BlockBuffer&, const SocketID&);

    unsigned int getNumBufferedMessages(const SocketID&) const;

    bool hasMessageToSend(const SocketID&) const;

    bool queueMessageToSend(const std::shared_ptr<MessageBase>&, const SocketID&);

    std::shared_ptr<MessageBase> popFirstMessageInQueue();
    std::shared_ptr<MessageBase> popMessageToSend(const SocketID&);

    //unsigned int removeSocketMessages(const SocketID&);

    int getInMessagePipeReadFD() const { return fdInMsgPipeRead_; }

  private:
    MessageBuffer();
    MessageBuffer(const MessageBuffer&) = delete;
    MessageBuffer& operator=(const MessageBuffer&) = delete;

  private:
    using MessageQueue = std::list<std::shared_ptr<MessageBase>>;
    MessageQueue queueIn_;
    MessageQueue queueOut_;

    using MsgBySocketID = std::map<SocketID, std::list<MessageQueue::iterator>>;
    MsgBySocketID inMsgBySocketID_;
    MsgBySocketID outMsgBySocketID_;

    int fdInMsgPipeRead_ = -1;
    int fdInMsgPipeWrite_ = -1;
};

}

#endif
