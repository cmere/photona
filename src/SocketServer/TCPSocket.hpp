#ifndef SOCKETSERVER_TCPSOCKET_H
#define SOCKETSERVER_TCPSOCKET_H

#include <string>
#include "ISelectable.hpp"

struct sockaddr_in;

namespace SocketServer
{

/**
 * IP version 4 socket.
 */
class TCPSocket : public ISelectable
{
  public:
    TCPSocket();
    TCPSocket(int fd, const std::string& peerIPAddress, unsigned int peerPort);
    virtual ~TCPSocket();

    int fd() const { return fd_; }
    bool isValid() const { return fd_ > 0; }

    virtual bool handleSelectReadable();
    virtual bool handleSelectWritable();

    const std::string& getPeerIPAddress() const { return peerIPAddress_; }
    const unsigned int getPeerPort() const { return peerPort_; }
    const std::string  getPeerPair() const { return peerIPAddress_ + ":" + std::to_string(peerPort_); }

    const std::string& getLocalIPAddress() const { return localIPAddress_; }
    const unsigned int getLocalPort() const { return localPort_; }
    const std::string  getLocalPair() const { return localIPAddress_ + ":" + std::to_string(localPort_); }

    bool connectTo(const std::string& serverIPAddress, unsigned int serverPort);

    void close();

  protected:
    void setLocalIPAddress_(const std::string& localIPAddress) { localIPAddress_ = localIPAddress; }
    void setLocalPort_(unsigned int localPort) { localPort_ = localPort; }
    std::string toIPString_(const sockaddr_in&);

  private:
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;

  protected:
    enum STATE { INITIAL, CREATED, CONNECTING, CONNECTED, CLOSED };

    int fd_ = -1;
    STATE state_ = INITIAL;
    std::string peerIPAddress_;
    unsigned int peerPort_ = -1;
    std::string localIPAddress_;
    unsigned int localPort_ = -1;
};

}

#endif // SOCKETSERVER_TCPSOCKET_H

