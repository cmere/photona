#include "ListenerSocketServer.hpp"
#include "ListenerTCPSocket.hpp"
#include "Logger.hpp"
#include "include/first.hpp"

#include <iostream>

using namespace std;
using namespace SocketServer;

int main(int argc, char *argv[])
{
  int exit_code = EXIT_SUCCESS;

  Logger::openLog("socketserver." + to_string(getpid()) + ".log");

  unsigned int port = ListenerTCPSocket::RANDOM_PORT;
  if (argc >= 1) {
    istringstream iss(argv[1]);
    if (!(iss >> port) || port <= 1024) {
      logger << "invalid port number " << argv[1] << endlog;
      return EXIT_FAILURE;
    }
  }

  ListenerSocketServer server;
  server.listenTo(ListenerTCPSocket::ANY_IPADDRESS, port);

  if (!server.run()) {
    exit_code = EXIT_FAILURE;
  }

  Logger::closeLog();
  return exit_code;
}

