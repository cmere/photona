#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "include/first.hpp"
#include "SocketLib/FDSelector.hpp"
#include "SocketLib/MessageBuffer.hpp"
#include "SocketLib/MessageEcho.hpp"
#include "SocketLib/MessageTest.hpp"
#include "SocketLib/SocketClient.hpp"
#include "SocketLib/TCPSocket.hpp"
#include "ClientMessageCenter.hpp"

using namespace std;
using namespace Client;
using namespace SocketLib;
using namespace Util;

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "Usage: socketserver_test <server ip address> <server port>" << endl;
    return EXIT_FAILURE;
  }

  logger.openLog("log.client");

  string serverIPAddress = argv[1];
  unsigned int serverPort = stoi(argv[2]);

  shared_ptr<ClientMessageCenter> pClientMsgCenter = ClientMessageCenter::GetSharedPtr();
  MessageCenter::SetSharedPtr(pClientMsgCenter);

  SocketClient socketClient;

  // create N socket, send one MessageEcho on each socket.
  {
    constexpr unsigned int N = 5;
    shared_ptr<TCPSocket> pSockets[N];
    for (unsigned int i = 0; i < N; ++i) {
      pSockets[i].reset(new TCPSocket());
      if (!pSockets[i]->connectTo(serverIPAddress, serverPort)) {
        logger << " socket connect failed: " << strerror(errno) << endlog;
        return EXIT_FAILURE;
      }
      //logger << logger.test << "fd=" << pSockets[i]->fd() << " connected " << pSockets[i]->getLocalPair() << " ->  " <<  pSockets[i]->getPeerPair() << endlog;
      auto pMsg = make_shared<MessageEcho>();
      pMsg->setContent("hello " + to_string(i));
      MessageBuffer::Singleton().queueMessageToSend(pMsg, pSockets[i]->getSocketID());
      socketClient.addTCPSocket(pSockets[i]);
    }
  }

  // send MessageTest 
  {
    auto pTestSocket = make_shared<TCPSocket>();
    if (!pTestSocket->connectTo(serverIPAddress, serverPort)) {
      logger << " socket connect failed: " << strerror(errno) << endlog;
      return EXIT_FAILURE;
    }

    socketClient.addTCPSocket(pTestSocket);

    // send message with unknow type, server should ignore msg.
    auto pMsg = make_shared<MessageTest>();
    /*
    pMsg->setData("Test unknown message type");
    pMsg->setMessageType(-1);
    MessageBuffer::Singleton().queueMessageToSend(pMsg, pTestSocket->getSocketID());

    // send message with truncated data, server should ignore data.
    pMsg = make_shared<MessageTest>();
    pMsg->setSendTruncatedData();
    MessageBuffer::Singleton().queueMessageToSend(pMsg, pTestSocket->getSocketID());

    // send normal data, server should receive data.
    pMsg = make_shared<MessageTest>();
    pMsg->setData("0123456789");
    MessageBuffer::Singleton().queueMessageToSend(pMsg, pTestSocket->getSocketID());
    */

    // send data with '\0'.
    pMsg = make_shared<MessageTest>();
    pMsg->setData(string(100*1024*1024, '\0').c_str(), 100*1024*1024);  // 1Mbytes of '\0'
    MessageBuffer::Singleton().queueMessageToSend(pMsg, pTestSocket->getSocketID());
  }

  int exitCode = EXIT_SUCCESS;
  if (!socketClient.run()) {
     exitCode = EXIT_FAILURE;
  }
  logger << logger.test << "exit " << exitCode << endlog;
  logger.closeLog();

  return exitCode;
}
