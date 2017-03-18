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

  // sent back to client
  MessageBuffer::Singleton().queueMessageToSend(pMsg, socketID);

  return 0;
}

}  // namespace Server
