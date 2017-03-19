#include "include/first.hpp"
#include "ServerMessageCenter.hpp"
#include "SocketLib/MessageBuffer.hpp"

using namespace std;
using namespace SocketLib;
using namespace Util;

namespace Server
{

std::shared_ptr<ServerMessageCenter>
ServerMessageCenter::GetSharedPtr()
{
  static shared_ptr<ServerMessageCenter> pObject;
  if (!pObject) {
    pObject.reset(new ServerMessageCenter());
  }

  return pObject;
}

int
ServerMessageCenter::handleSelectReadable()
{
  char c;
  if (::read(fd(), &c, 1) != 1) {
    logger << "ServerMessageCenter failed to read from pipe." << endlog;
  }
  auto socketID_pMsg = MessageBuffer::Singleton().popFirstMessageInQueue();
  auto socketID = socketID_pMsg.first;
  auto pMsg = socketID_pMsg.second;
  if (!pMsg) {
    logger << "ServerMessageCenter failed to pop message from MessageBuffer." << endlog;
    return 0;
  }

  logger << logger.test << "ServerMessageCenter socketID=" << socketID << " received " << pMsg->getName() << endlog;
  if (   pMsg->getType() == MessageBase::TEcho 
      || pMsg->getType() == MessageBase::TTest) {
    logger << logger.test << "send back" << endlog;
    MessageBuffer::Singleton().queueMessageToSend(pMsg, socketID);
  }
  else {
    logger << logger.test << "message type not support " << pMsg->getType() << endlog;
  }

  return 0;
}

}  // namespace Server
