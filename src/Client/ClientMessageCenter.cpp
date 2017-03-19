#include "include/first.hpp"
#include "ClientMessageCenter.hpp"
#include "SocketLib/MessageBuffer.hpp"
#include "SocketLib/MessageEcho.hpp"
#include "SocketLib/MessageTest.hpp"

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
  
  logger << logger.test << "ClientMessageCenter socketID=" << socketID << " received " << pMsg->getName() << endlog;
  if (pMsg->getType() == MessageBase::TEcho) {
    logger << logger.test << "content=[" << (static_cast<MessageEcho*>(pMsg.get()))->getContent() << "]" << endlog;
  }
  else if (pMsg->getType() == MessageBase::TTest) {
    logger << logger.test << "length=[" << (static_cast<MessageTest*>(pMsg.get()))->getDataLength() << "]" << endlog;
  }
  
  finishedSocketIDs_.insert(socketID);

  return 0;
}

}  // namespace Client
