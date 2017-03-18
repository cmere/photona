#ifndef UTIL_LOGGER_HPP
#define UTIL_LOGGER_HPP

#include <fstream>
#include <sstream>

namespace Util 
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
    static void openLog(const std::string& logFileName);
    static void closeLog();

    static void enableDebug(bool);
    static void enableTest(bool);

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
