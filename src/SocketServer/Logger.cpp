#include "Logger.hpp"
#include "include/first.hpp"
#include "boost/locale/time_zone.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <ctime>
#include <iostream>
#include <memory>

using namespace std;
namespace bst = boost::posix_time;

namespace {

// implementation
string LOG_DIR = "/var/log/photona/";
ofstream ofs;
unsigned int logsize = 0;
unsigned int LogSizeMax = 300 * 1024 * 1024;  // 300M bytes.

int logtype = 0;
enum LogType { 
  LogType_Normal = 0,
  LogType_Debug  = 1,
  LogType_Test   = 2,
  LogType_Fatal  = 3,
};

string LogType_S[] = { " ", " [DEBUG] ", " [TEST] ", " [FATAL] " };
bool LogType_Enabled[] = { true, true, true, true };

}  // namespace {

namespace SocketServer 
{

Logger logger;

void
Logger::openLog(const std::string& logFilename)
{
  if (ofs.is_open()){
    ofs.close();
  }
  ofs.open((LOG_DIR + logFilename).c_str(), ios_base::out | ios_base::app);
  logsize = 0;

  time_t t = time(0);
  string tz = ::localtime(&t)->tm_zone;
  bst::ptime now(bst::second_clock::local_time());
  logger << "log opened " << bst::to_iso_string(now) << " " << tz << endlog;
}

void
Logger::closeLog() 
{
  if (!ofs.is_open()) {
    return;
  }

  time_t t = time(0);
  string tz = ::localtime(&t)->tm_zone;
  bst::ptime now(bst::second_clock::local_time());
  logger << "log closed " << bst::to_iso_string(now) << " " << tz << endlog;

  size_t logsizeNumOfDigit = to_string(logsize).size();
  logsize += string("HH:MM:SS.mmm log size=").size() + to_string(logsize).size() + 1;

  // in case logsize was 98 after "log closed ", "98" has 2 digits, so logsize + 2. 
  // Actually logsize should + 3 because logsize is more than 100 (3 digits).
  if (to_string(logsize).size() != logsizeNumOfDigit) {
    logsize += 1;
  }

  logger << "log size=" << logsize << endlog;
  ofs.close();
}

ostream&
endlog(ostream& os)
{
  Logger* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    const string& str = pLogger->str();
    // write to log file.
    if (LogType_Enabled[logtype]) {
      bst::ptime now(bst::microsec_clock::local_time());
      auto t = now.time_of_day();
      ostringstream timestamp;
      timestamp << setfill('0');
      timestamp << setw(2) << t.hours() << ":";
      timestamp << setw(2) << t.minutes() << ":";
      timestamp << setw(2) << t.seconds() << ".";
      timestamp << setw(3) << t.fractional_seconds() / 1000;

      if (ofs.is_open()) {
        ofs << timestamp.str() << LogType_S[logtype] << str << std::endl;
      }
      else {
        cout << timestamp.str() << LogType_S[logtype] << str << std::endl;
      }
      logsize += timestamp.str().size() + LogType_S[logtype].size() + str.size() + 1;  // NOTE: Windows has two char for newline "\r\n".

      if (logsize >= LogSizeMax && ofs.is_open()) {
        string strExceedLogSize = timestamp.str() + " log size " + to_string(logsize) + " exceeds limit " + to_string(LogSizeMax) + ". stop logging.";
        ofs << strExceedLogSize << std::endl;
        ofs.close();
      }
    }
    pLogger->str("");

    // abort if Fatal.
    if (logtype == LogType_Fatal) {
      abort();
    }

    logtype = LogType_Normal;
  }
  else {
    os << endl;
  }

  return os;
}


ostream&
Logger::debug(ostream& os)
{
  Logger* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    logtype = LogType_Debug;
  }

  return os;
}

ostream&
Logger::test(ostream& os)
{
  Logger* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    logtype = LogType_Test;
  }

  return os;
}

ostream&
Logger::fatal(ostream& os)
{
  Logger* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    logtype = LogType_Fatal;
  }

  return os;
}

void
Logger::enableDebug(bool value) {
  LogType_Enabled[LogType_Debug] = value;
}

void
Logger::enableTest(bool value) {
  LogType_Enabled[LogType_Test] = value;
}

}
