#ifndef SOCKETSERVER_TCPSOCKET_H
#define SOCKETSERVER_TCPSOCKET_H

#include <deque>
#include <memory>
#include <string>
#include <vector>
#include "SocketBase.hpp"
#include "BlockBuffer.hpp"

namespace SocketServer
{

/**
 * Two kinds of sockets: client socket connect to server; socket accepted by a listen socker.
 */
class TCPSocket : public SocketBase
{
  public:
    TCPSocket();
    TCPSocket(int fd, const std::string& peerIPAddress, unsigned int peerPort);

    virtual ~TCPSocket() { }
    virtual void close();

    virtual int handleSelectReadable();
    virtual int handleSelectWritable();

    bool hasBytesToSend() const;

    bool connectTo(const std::string& serverIPAddress, unsigned int serverPort);

  private:
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;

    int checkNonBlockConnect_();

  private:
    BlockBuffer recvBuffer_;
    BlockBuffer sendBuffer_;

    bool isConnected_ = false;
};

}

#endif // SOCKETSERVER_TCPSOCKET_H

