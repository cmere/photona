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

    // manipulator to write log msg. (see std::endl)
    static std::ostream& end(std::ostream& os);
    static std::ostream& debug(std::ostream& os);
    static std::ostream& test(std::ostream& os);
    static std::ostream& fatal(std::ostream& os);
};

extern Logger logger;

std::ostream& endlog(std::ostream& os);


}

#endif
