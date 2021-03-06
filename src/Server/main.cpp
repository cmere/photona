#include "include/first.hpp"
#include "ServerMessageCenter.hpp"
#include "SocketLib/SocketServer.hpp"

#include <iostream>

using namespace std;
using namespace SocketLib;
using namespace Server;
using namespace Util;

int main(int argc, char *argv[])
{
  int exit_code = EXIT_SUCCESS;

  if (argc <= 1) {
    cout << "usage: \nsocketserver <port>" << endl;
    return -1;
  }

  Logger::openLog("log.photona.server");

  unsigned int port = 0;
  istringstream iss(argv[1]);
  if (!(iss >> port) || port <= 1024) {
    logger << logger.fatal << "invalid port number " << argv[1] << endlog;
  }

  shared_ptr<ServerMessageCenter> pServerMsgCenter = ServerMessageCenter::GetSharedPtr();
  MessageCenter::SetSharedPtr(pServerMsgCenter);

  SocketServer server;
  if (!server.run("*", port)) {
    exit_code = EXIT_FAILURE;
  }

  Logger::closeLog();
  return exit_code;
}

