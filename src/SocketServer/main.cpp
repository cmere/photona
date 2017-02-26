#include "TCPServer.hpp"
#include "ListenerTCPSocket.hpp"
#include "Logger.hpp"
#include "include/first.hpp"

#include <iostream>

using namespace std;
using namespace SocketServer;

int main(int argc, char *argv[])
{
  int exit_code = EXIT_SUCCESS;

  Logger::openLog("log.socketserver." + to_string(getpid()));

  if (argc <= 1) {
    cout << "usage: \nsocketserver <port>" << endl;
    return -1;
  }

  unsigned int port = 0;
  istringstream iss(argv[1]);
  if (!(iss >> port) || port <= 1024) {
    logger << logger.fatal << "invalid port number " << argv[1] << endlog;
  }

  TCPServer server;
  server.listenTo(SocketBase::ANY_IPADDRESS, port);

  if (!server.run()) {
    exit_code = EXIT_FAILURE;
  }

  Logger::closeLog();
  return exit_code;
}

