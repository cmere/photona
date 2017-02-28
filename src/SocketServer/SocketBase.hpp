#ifndef SOCKETSERVER_SOCKETBASE_H
#define SOCKETSERVER_SOCKETBASE_H

#include <string>
#include "ISelectable.hpp"
#include "SocketID.hpp"

struct sockaddr_in;

namespace SocketServer
{

/**
 * Maintain socket fd, local/peer address.
 */
class SocketBase : public ISelectable
{
  public:
    static std::string ANY_IPADDRESS;
    static unsigned int RANDOM_PORT;

    SocketBase(int socketType);
    SocketBase(int fd, const std::string& peerIPAddress, unsigned int peerPort);
    virtual ~SocketBase();

    int fd() const { return fd_; }
    bool isValid() const { return fd_ > 0; }
    virtual void close();

    const std::string& getPeerIPAddress() const { return peerIPAddress_; }
    const unsigned int getPeerPort() const { return peerPort_; }
    const std::string  getPeerPair() const { return peerIPAddress_ + ":" + std::to_string(peerPort_); }

    const std::string& getLocalIPAddress() const { return localIPAddress_; }
    const unsigned int getLocalPort() const { return localPort_; }
    const std::string  getLocalPair() const { return localIPAddress_ + ":" + std::to_string(localPort_); }
    const SocketID& getSocketID() const { return socketID_; }

  protected:
    bool bind_(const std::string& localIPAddress, unsigned int localPort);

    void setLocalIPAddress_(const std::string& localIPAddress) { localIPAddress_ = localIPAddress; }
    void setLocalPort_(unsigned int localPort) { localPort_ = localPort; }

    std::string toIPString_(const sockaddr_in&);
    bool toSockAddr_(const std::string&, unsigned int, sockaddr_in&);

  private:
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;

  protected:
    unsigned int socketID_;
    int fd_ = -1;
    std::string peerIPAddress_;
    unsigned int peerPort_ = -1;
    std::string localIPAddress_;
    unsigned int localPort_ = -1;
    static SocketID staticSocketID_;
};

}

#endif
