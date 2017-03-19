#ifndef SOCKETLIB_SOCKETCLIENT_H
#define SOCKETLIB_SOCKETCLIENT_H

#include "SocketID.hpp"
#include <map>
#include <memory>

namespace SocketLib
{

class TCPSocket;

/**
 * Embed in client application.
 */
class SocketClient
{
  public:
    SocketClient() { }

    bool run();

    void addTCPSocket(std::shared_ptr<TCPSocket>);

  private:
    SocketClient(const SocketClient&) = delete;
    SocketClient& operator=(const SocketClient&) = delete;

  private:
    std::map<SocketID, std::shared_ptr<TCPSocket>> pSockets_;
};

}

#endif // SOCKETLIB_SOCKETCLIENT_H
