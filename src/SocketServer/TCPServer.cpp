#include "include/first.hpp"
#include "FDSelector.hpp"
#include "TCPServer.hpp"
#include "ListenerTCPSocket.hpp"
#include "TCPSocket.hpp"
#include "WorkerSocketServer.hpp"

#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

namespace SocketServer 
{

static unsigned int numOfChildProcess = 0;
static unsigned int MaxNumOfWorkerServer = 1;

void sighandler_child(int sid)
{
  int status = 0;
  int childPID = ::waitpid(-1, &status, WNOHANG);
  if (childPID == -1) {
    logger << "wait on child failed." << endlog;
  }
  else {
    logger << "child pid=" << childPID << " exit with status " << status << endlog;
    --numOfChildProcess;
  }
}

/**
 * 
 */
class SignalHandler
{
public:
  SignalHandler(int signum, void (*sighandler)(int))
  { 
    if (signum < 0 || !sighandler) {
      logger << "invalid signal handler." << endlog;
      return;
    }

    struct sigaction newact;
    memset(&newact, 0, sizeof(newact));
    newact.sa_handler = sighandler;
    if (::sigaction(signum, &newact, &oldact_) == -1) {
      logger << "set signal handler failed: " << signum << " " << strerror(errno) << endlog;
      signum_ = -1;
    }
  }

  ~SignalHandler()
  {
    if (signum_ > 0) {
      if (::sigaction(signum_, &oldact_, nullptr) == -1) {
        logger << "reset signal handler failed: " << signum_ << " " << strerror(errno) << endlog;
      }
    }
  }

private:
  int signum_ = -1;
  struct sigaction oldact_;
};

bool
TCPServer::listenTo(const string& ipaddress, unsigned int port)
{
  pListenerTCPSocket_ = make_shared<ListenerTCPSocket>();
  if (!pListenerTCPSocket_->bindAndListen(ipaddress, port)) {
    logger << "socket bind/listen failed: ipaddress=" << ipaddress << " port=" << port << endlog;
    return false;
  }
  return true;
}

bool
TCPServer::run() 
{
  if (!pListenerTCPSocket_ || !pListenerTCPSocket_->isValid()) {
    logger << "listener socket is not ready. exit." << endlog;
    return false;
  }

  logger << "listening " << pListenerTCPSocket_->getLocalIPAddress() << ":" << pListenerTCPSocket_->getLocalPort() << endlog;

  SignalHandler(SIGCHLD, &sighandler_child);

  FDSelector selector;
  selector.addToReadSelectable(pListenerTCPSocket_);
  
  while (1) {
    if (!pListenerTCPSocket_->isValid()) {
      logger << "listener socket is invalid. exit." << endlog;
      return false;
    }

    auto retval = selector.select();  // block waiting
    if (retval <= 0) {
      logger << "socket select errror. exit." << endlog;
      return false;
    }

    const auto& readyToReadSockets = selector.getReadyToRead();
    if (readyToReadSockets.size() != 1 || *readyToReadSockets.begin() != pListenerTCPSocket_) {
      logger << "select a wrong listener socket. exit. " << (*readyToReadSockets.begin())->fd() << ":" << pListenerTCPSocket_->fd() << endlog;
      return false;
    }

    if (pListenerTCPSocket_->handleSelectReadable() < 0) {
      logger << "failed to handle read FD in select. fd=" << pListenerTCPSocket_->fd() << endlog;
      return false;
    }

    shared_ptr<TCPSocket> pClientSocket = move(pListenerTCPSocket_->getAcceptedClient());
    if (!pClientSocket) {
      logger << "faled to accept client socket. exit." << endlog;
      return false;
    }

    if (numOfChildProcess >= MaxNumOfWorkerServer) {
      logger << "ignore client " << pClientSocket->getPeerPair() 
             << " due to too many connected clients " << numOfChildProcess << " limit=" << MaxNumOfWorkerServer << endlog;
      pClientSocket->close();
      continue;
    }

    // start child process as worker socket server.
    pid_t childPID = fork();
    if (childPID > 0) {  // parent process
      logger << "started child process " << childPID << " for client " << pClientSocket->getPeerPair() << endlog;
      pClientSocket->close();
      numOfChildProcess++;
    }
    else if (childPID == 0) {  // child process
      Logger::openLog("socketserver." + to_string(getpid()) + "." 
                      + pClientSocket->getPeerIPAddress() + "." + to_string(pClientSocket->getPeerPort()) + ".log");
      pListenerTCPSocket_->close();
      logger << "child pid=" << getpid() << " serve client " << pClientSocket->getPeerPair() << endlog;
      WorkerSocketServer workerServer(pClientSocket);
      workerServer.run();
      logger << "child pid=" << getpid() << " serve client " << pClientSocket->getPeerPair() << " DONE" << endlog;
      //Logger::closeLog();  close log in main()
      return true;
    }
    else {  // create child process failed
      logger << "failed to spawn worker socket server process." << endlog;
    }
  }

  return true;
}

}
