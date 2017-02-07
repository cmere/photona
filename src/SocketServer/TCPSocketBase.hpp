#ifndef SOCKETSERVER_TCPSOCKETBASE_H
#define SOCKETSERVER_TCPSOCKETBASE_H

#include <string>
#include "ISelectable.hpp"

struct sockaddr_in;

namespace SocketServer
{

/**
 * Maintain socket fd, local/peer address.
 */
class TCPSocketBase : public ISelectable
{
  public:
    TCPSocketBase();
    TCPSocketBase(int fd, const std::string& peerIPAddress, unsigned int peerPort);
    virtual ~TCPSocketBase();

    int fd() const { return fd_; }
    bool isValid() const { return fd_ > 0; }
    virtual void close();

    const std::string& getPeerIPAddress() const { return peerIPAddress_; }
    const unsigned int getPeerPort() const { return peerPort_; }
    const std::string  getPeerPair() const { return peerIPAddress_ + ":" + std::to_string(peerPort_); }

    const std::string& getLocalIPAddress() const { return localIPAddress_; }
    const unsigned int getLocalPort() const { return localPort_; }
    const std::string  getLocalPair() const { return localIPAddress_ + ":" + std::to_string(localPort_); }
    const std::string  getClientID() const { return getLocalPair(); }

  protected:
    void setLocalIPAddress_(const std::string& localIPAddress) { localIPAddress_ = localIPAddress; }
    void setLocalPort_(unsigned int localPort) { localPort_ = localPort; }

    std::string toIPString_(const sockaddr_in&);

  private:
    TCPSocketBase(const TCPSocketBase&) = delete;
    TCPSocketBase& operator=(const TCPSocketBase&) = delete;

  protected:
    int fd_ = -1;
    std::string peerIPAddress_;
    unsigned int peerPort_ = -1;
    std::string localIPAddress_;
    unsigned int localPort_ = -1;
};

}

#endif

