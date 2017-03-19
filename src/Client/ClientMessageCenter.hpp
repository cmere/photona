#ifndef SERVER_CLIENTMESSAGECENTER_HPP
#define SERVER_CLIENTMESSAGECENTER_HPP

#include <memory>
#include "SocketLib/MessageCenter.hpp"

namespace Client
{

/**
 * Message center for client.
 */
class ClientMessageCenter : public SocketLib::MessageCenter
{
  public:
    static std::shared_ptr<ClientMessageCenter> GetSharedPtr();

    // from MessageCenter
    virtual int handleSelectReadable();
    virtual std::set<SocketLib::SocketID> getFinishedSocketIDs() { return finishedSocketIDs_; }
    virtual void clearFinishedSocketIDs() { finishedSocketIDs_.clear(); }

  protected:
    ClientMessageCenter() { }

  private:
    std::set<SocketLib::SocketID> finishedSocketIDs_;
};

}  // namespace Client

#endif //SERVER_CLIENTMESSAGECENTER_HPP
