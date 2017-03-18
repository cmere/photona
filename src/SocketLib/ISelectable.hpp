#ifndef SOCKETSERVER_ISELECTABLE_H
#define SOCKETSERVER_ISELECTABLE_H

#include "SocketID.hpp"

namespace SocketServer
{

class ISelectable
{
  public:
    virtual int fd() const = 0;
    virtual void close() = 0;
    virtual bool isValid() const = 0;
    virtual int handleSelectReadable() = 0;
    virtual int handleSelectWritable() = 0;
    virtual const SocketID& getSocketID() const = 0;
};

}

#endif // SOCKETSERVER_ISELECTABLE_H
