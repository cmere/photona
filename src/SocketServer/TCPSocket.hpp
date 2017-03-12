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

  private:
    std::string strBuffer_;
    std::vector<char> vecBuffer_;
    std::deque<char> dequeBuffer_;

    BlockBuffer blockBuffer_;

    std::unique_ptr<char> pBytesNotExtracted_;  // after read(), some bytes are not extracted into messages.
    unsigned int numBytesNotExtracted_ = 0;

    std::unique_ptr<char> pBytesNotSend_;  // after write(), a part of a message are sent, some bytes are left.
    unsigned int numBytesNotSend_ = 0;
};

}

#endif // SOCKETSERVER_TCPSOCKET_H

