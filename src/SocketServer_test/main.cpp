#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "include/first.hpp"
#include "SocketServer/FDSelector.hpp"
#include "SocketServer/MessageBuffer.hpp"
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

  string serverIPAddress = argv[1];
  unsigned int serverPort = stoi(argv[2]);

  constexpr unsigned int N = 5;
  shared_ptr<TCPSocket> pSockets[N];
  FDSelector selector;

  for (unsigned int i = 0; i < N; ++i) {
    pSockets[i].reset(new TCPSocket());
    cout << "connecting to " << serverIPAddress << ":" << serverPort << endl;
    if (!pSockets[i]->connectTo(serverIPAddress, serverPort)) {
      cout << "socket connect failed: " << strerror(errno) << endl;
      return EXIT_FAILURE;
    }
    cout << "fd=" << pSockets[i]->fd() << " connected " << pSockets[i]->getLocalPair() << " ->  " <<  pSockets[i]->getPeerPair() << endl;
    auto pMsg = make_shared<MessageTest>(pSockets[i]->getClientID());
    pMsg->setBody("hello " + to_string(i));
    MessageBuffer::Singleton().queueMessageToSend(pMsg);
    selector.addToReadSelectable(pSockets[i]);
    selector.addToWriteSelectable(pSockets[i]);
  }

  while (1) {
    int sel = selector.select();
    cout << "select() = " << sel << endl;
    if (sel <= 0) {
      return EXIT_FAILURE;
    }

    auto pReadSockets = selector.getReadyToRead();
    cout << "socket ready to read " << pReadSockets.size() << endl;
    auto pWriteSockets = selector.getReadyToWrite();
    cout << "socket ready to write " << pWriteSockets.size() << endl;

    for (auto pReadSocket : pReadSockets) {
      if (pReadSocket->handleSelectReadable() <= 0) {  // eof or error
        pWriteSockets.erase(pReadSocket);
      }
    }
    for (auto pWriteSocket : pWriteSockets) {
      pWriteSocket->handleSelectWritable();
    }
  }

  return EXIT_SUCCESS;
}
