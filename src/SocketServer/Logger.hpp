#ifndef SOCKETSERVER_LOGGER_HPP
#define SOCKETSERVER_LOGGER_HPP

#include <fstream>
#include <sstream>

namespace SocketServer 
{

/**
 * Usage: 
 *
 * Logger::openLog("log_filename");  // start of process
 * logger << "..." << 123 << endlog;
 * Logger::closeLog();  // before exit process
 */
class Logger : public std::ostringstream
{
  public:
    static void openLog(const std::string& logFileKey);
    static void closeLog();

  private:
    void writeLog_(const std::string&);

  friend std::ostream& endlog(std::ostream&);

  private:
    static std::ofstream ofs_;
};

// manipulator to write log msg. (see std::endl)
std::ostream& endlog(std::ostream& os);

extern Logger logger;

}

#endif
