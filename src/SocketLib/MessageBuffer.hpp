#ifndef SOCKETSERVER_MESSAGEBUFFER_HPP
#define SOCKETSERVER_MESSAGEBUFFER_HPP

#include <map>
#include <memory>
#include <list>
#include "MessageBase.hpp"
#include "SocketID.hpp"

namespace SocketServer
{

class BlockBuffer;

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
    unsigned int extractMessageFromSocket(BlockBuffer&, const SocketID&);

    unsigned int getNumBufferedMessages(const SocketID&) const;

    bool hasMessageToSend(const SocketID&) const;

    bool queueMessageToSend(const std::shared_ptr<MessageBase>&, const SocketID&);

    std::shared_ptr<MessageBase> popMessageToSend(const SocketID&);

    //unsigned int removeSocketMessages(const SocketID&);

    int getReadFD() const { return fdRead_; }

  private:
    MessageBuffer();
    MessageBuffer(const MessageBuffer&) = delete;
    MessageBuffer& operator=(const MessageBuffer&) = delete;

  private:
    using MessageQueueType = std::list<std::shared_ptr<MessageBase>>;
    MessageQueueType queueIn_;
    MessageQueueType queueOut_;

    std::map<SocketID, std::list<MessageQueueType::iterator>> inMsgBySocketID_;
    std::map<SocketID, std::list<MessageQueueType::iterator>> outMsgBySocketID_;

    int fdRead_;
    int fdWrite_;
};

}

#endif
