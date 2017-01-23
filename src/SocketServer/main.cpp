#include "ListenerSocketServer.hpp"
#include "ListenerTCPSocket.hpp"

using namespace std;
using namespace SocketServer;

int main(int argc, char *argv[])
{
  int exit_code = EXIT_SUCCESS;

  ListenerSocketServer server;
  server.listenTo(ListenerTCPSocket::ANY_IPADDRESS, ListenerTCPSocket::RANDOM_PORT);

  if (!server.run()) {
    exit_code = EXIT_FAILURE;
  }

  return exit_code;
}

