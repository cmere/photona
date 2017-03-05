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

  while (1) {
    int sel = selector.select();
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
    break;
  }

  logger.closeLog();
  return EXIT_SUCCESS;
}
