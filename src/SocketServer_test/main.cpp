#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "include/first.hpp"
#include "SocketServer/FDSelector.hpp"
#include "SocketServer/Logger.hpp"
#include "SocketServer/MessageBuffer.hpp"
#include "SocketServer/MessageEcho.hpp"
#include "SocketServer/MessageTest.hpp"
#include "SocketServer/TCPSocket.hpp"

using namespace std;
using namespace SocketServer;

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "Usage: socketserver_test <server ip address> <server port>" << endl;
    return EXIT_FAILURE;
  }

  logger.openLog("log.socketserver_test");

  string serverIPAddress = argv[1];
  unsigned int serverPort = stoi(argv[2]);

  constexpr unsigned int N = 5;
  shared_ptr<TCPSocket> pSockets[N];
  FDSelector selector;

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

  // send MessageTest 
  {
    auto pTestSocket = make_shared<TCPSocket>();
    if (!pTestSocket->connectTo(serverIPAddress, serverPort)) {
      logger << " socket connect failed: " << strerror(errno) << endlog;
      return EXIT_FAILURE;
    }
    logger << logger.test << "fd=" << pTestSocket->fd() << " connected " << pTestSocket->getLocalPair() << " ->  " <<  pTestSocket->getPeerPair() << endlog;
    selector.addToReadSelectable(pTestSocket);
    selector.addToWriteSelectable(pTestSocket);

    // send message with unknow type, server should ignore msg.
    auto pMsg = make_shared<MessageTest>();
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
  }

  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while (1) {
    int sel = selector.select(&timeout);
    logger << logger.debug << "select() = " << sel << endlog;
    if (sel <= 0) {
      logger << "select return 0, exit" << endlog;
      break;
    }

    auto pReadSockets = selector.getReadyToRead();
    logger << logger.debug << "number socket ready to read = " << pReadSockets.size() << endlog;
    auto pWriteSockets = selector.getReadyToWrite();
    logger << logger.debug << "number socket ready to write = " << pWriteSockets.size() << endlog;

    for (auto pReadSocket : pReadSockets) {
      if (pReadSocket->handleSelectReadable() <= 0) {  // eof or error
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
