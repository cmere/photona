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

  ListenerSocketServer server;
  server.listenTo(ListenerTCPSocket::ANY_IPADDRESS, ListenerTCPSocket::RANDOM_PORT);

  if (!server.run()) {
    exit_code = EXIT_FAILURE;
  }

  Logger::closeLog();
  return exit_code;
}

