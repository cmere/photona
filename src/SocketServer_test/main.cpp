#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "include/first.hpp"
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

  TCPSocket tcpSocket;
  cout << "connecting to " << serverIPAddress << ":" << serverPort << endl;
  if (!tcpSocket.connectTo(serverIPAddress, serverPort)) {
    cout << "socket connect failed: " << strerror(errno) << endl;
    return EXIT_FAILURE;
  }

  cout << "fd=" << tcpSocket.fd() << " connected to " << serverIPAddress << ":" << serverPort << endl;
  ::sleep(1);

  return EXIT_SUCCESS;
}
