#include "include/first.hpp"
#include "Logger.hpp"
#include "SocketBase.hpp"
#include "SocketServer.hpp"

#include <iostream>

using namespace std;
using namespace SocketServer;

int main(int argc, char *argv[])
{
  int exit_code = EXIT_SUCCESS;

  if (argc <= 1) {
    cout << "usage: \nsocketserver <port>" << endl;
    return -1;
  }

  Logger::openLog("log.socketserver");

  unsigned int port = 0;
  istringstream iss(argv[1]);
  if (!(iss >> port) || port <= 1024) {
    logger << logger.fatal << "invalid port number " << argv[1] << endlog;
  }

  SocketServer::SocketServer server;
  if (!server.run(SocketBase::ANY_IPADDRESS, port)) {
    exit_code = EXIT_FAILURE;
  }

  Logger::closeLog();
  return exit_code;
}

