#ifndef SOCKETSERVER_MESSAGEBUFFER_HPP
#define SOCKETSERVER_MESSAGEBUFFER_HPP

#include <map>
#include <memory>
#include <list>
#include "MessageBase.hpp"

namespace SocketServer
{

/**
 * Input/output buffer for messages. All in/out socket share this one quque (FIFO).
 *
 * Socket read bytes, send here to construct messages, and save to input queue. 
 * If there are messages in output queue, socket send them.
 */
class MessageBuffer
{
  public:
    static MessageBuffer& Singleton();

    // extract ONE message, put in queue, return number of bytes extracted.
    unsigned int extractMessageFromBytes(const char*, unsigned int length);

    bool needReadMore(const MessageBase::From&) const;

    bool hasMessageToSend(MessageBase::To) const;
    std::shared_ptr<MessageBase> popMessage(MessageBase::To);

    bool queueMessageToSend(const std::shared_ptr<MessageBase>);

  private:
    MessageBuffer();
    MessageBuffer(const MessageBuffer&) = delete;
    MessageBuffer& operator=(const MessageBuffer&) = delete;

  private:
    using MessageQueueType = std::list<std::shared_ptr<MessageBase>>;
    MessageQueueType queueIn_;
    MessageQueueType queueOut_;

    std::map<MessageBase::From, MessageQueueType::iterator> inMsgBySrc_;
    std::map<MessageBase::To, MessageQueueType::iterator>   outMsgByDest_;

};

}

#endif
