#include "include/first.hpp"
#include "ClientMessageCenter.hpp"
#include "SocketLib/MessageBuffer.hpp"

using namespace std;
using namespace SocketLib;
using namespace Util;

namespace Client
{

std::shared_ptr<ClientMessageCenter>
ClientMessageCenter::GetSharedPtr()
{
  static shared_ptr<ClientMessageCenter> pObject;
  if (!pObject) {
    pObject.reset(new ClientMessageCenter());
  }

  return pObject;
}

int
ClientMessageCenter::handleSelectReadable()
{
  char c;
  if (::read(fd(), &c, 1) != 1) {
    logger << "ClientMessageCenter failed to read from pipe." << endlog;
  }
  auto socketID_pMsg = MessageBuffer::Singleton().popFirstMessageInQueue();
  auto socketID = socketID_pMsg.first;
  auto pMsg = socketID_pMsg.second;
  if (!pMsg) {
    logger << "ClientMessageCenter failed to pop message from MessageBuffer." << endlog;
    return 0;
  }
  
  finishedSocketIDs_.insert(socketID);

  return 0;
}

}  // namespace Client
