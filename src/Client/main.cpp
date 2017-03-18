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
#include "SocketLib/TCPSocket.hpp"

using namespace std;
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

  constexpr unsigned int N = 5;
  shared_ptr<TCPSocket> pSockets[N];
  FDSelector selector;

  /*
  // create N socket, send one MessageEcho on each socket.
  for (unsigned int i = 0; i < N; ++i) {
    pSockets[i].reset(new TCPSocket());
    if (!pSockets[i]->connectTo(serverIPAddress, serverPort)) {
      logger << " socket connect failed: " << strerror(errno) << endlog;
      return EXIT_FAILURE;
    }
    logger << logger.test << "fd=" << pSockets[i]->fd() << " connected " << pSockets[i]->getLocalPair() << " ->  " <<  pSockets[i]->getPeerPair() << endlog;
    auto pMsg = make_shared<MessageEcho>();
    pMsg->setContent("hello " + to_string(i));
    MessageBuffer::Singleton().queueMessageToSend(pMsg, pSockets[i]->getSocketID());
    selector.addToReadSelectable(pSockets[i]);
    selector.addToWriteSelectable(pSockets[i]);
  }
  */

  // send MessageTest 
  {
    auto pTestSocket = make_shared<TCPSocket>();
    if (!pTestSocket->connectTo(serverIPAddress, serverPort)) {
      logger << " socket connect failed: " << strerror(errno) << endlog;
      return EXIT_FAILURE;
    }
    selector.addToReadSelectable(pTestSocket);
    selector.addToWriteSelectable(pTestSocket);

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

  //timeval timeout;
  //timeout.tv_sec = 1;
  //timeout.tv_usec = 0;

  while (1) {
    int sel = selector.select();
    if (sel <= 0) {
      logger << "select return 0, exit" << endlog;
      break;
    }

    auto pReadSockets = selector.getReadyToRead();
    auto pWriteSockets = selector.getReadyToWrite();

    for (auto pReadSocket : pReadSockets) {
      if (pReadSocket->handleSelectReadable() <= 0 && pReadSocket->fd() < 0) {  // eof or error
        pWriteSockets.erase(pReadSocket);
      }
    }
    for (auto pWriteSocket : pWriteSockets) {
      pWriteSocket->handleSelectWritable();
    }
  }

  logger.closeLog();
  return EXIT_SUCCESS;
}