#ifndef SOCKETLIB_UDPMESSAGEBUFFER_HPP
#define SOCKETLIB_UDPMESSAGEBUFFER_HPP

#include <map>
#include <memory>
#include <list>
#include <tuple>
#include "MessageBase.hpp"
#include "SocketID.hpp"

namespace SocketLib
{

/**
 * Similar to MessageBuffer (for TCP), but also need keep peer's address.
 * is not connected.
 */
class UDPMessageBuffer
{
  public:
    static UDPMessageBuffer& Singleton();

    // extract ONE message, put in queue, return number of bytes extracted.
    unsigned int extractMessageFromSocket(const char*, unsigned int length, const SocketID&, const std::string& peerIP, const unsigned int peerPort);

    bool shouldReadMoreOnSocket(const SocketID&) const;

    bool hasMessageToSend(const SocketID&) const;

    bool queueMessageToSend(const std::shared_ptr<MessageBase>&, const SocketID&, const std::string& peerIP, const unsigned int peerPort);

    std::tuple<std::shared_ptr<MessageBase>, std::string, unsigned int> popMessageToSend(const SocketID&);

  private:
    UDPMessageBuffer();
    UDPMessageBuffer(const UDPMessageBuffer&) = delete;
    UDPMessageBuffer& operator=(const UDPMessageBuffer&) = delete;

  private:
    using MessageQueueType = std::list< std::tuple<std::shared_ptr<MessageBase>, std::string, unsigned int> >;
    MessageQueueType queueIn_;
    MessageQueueType queueOut_;

    std::map<SocketID, std::list<MessageQueueType::iterator>> inMsgBySocketID_;
    std::map<SocketID, std::list<MessageQueueType::iterator>> outMsgBySocketID_;
};

}

#endif
