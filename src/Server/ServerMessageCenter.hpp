#ifndef SERVER_SERVERMESSAGECENTER_HPP
#define SERVER_SERVERMESSAGECENTER_HPP

#include <memory>
#include "SocketLib/MessageCenter.hpp"

namespace Server
{

/**
 * Message center for server.
 */
class ServerMessageCenter : public SocketLib::MessageCenter
{
  public:
    static std::shared_ptr<ServerMessageCenter> GetSharedPtr();

    virtual int handleSelectReadable();

  protected:
    ServerMessageCenter() { }
};

}  // namespace Server

#endif //SERVER_SERVERMESSAGECENTER_HPP
