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
int logtype = 0;
enum LogType { 
  LogType_Normal = 0,
  LogType_Debug  = 1,
  LogType_Test   = 2,
  LogType_Fatal  = 3,
};

string LogType_S[] = { " ", " [DEBUG] ", " [TEST] ", " [FATAL] " };

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
  ofs.open((LOG_DIR + logFilename).c_str());

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
  ofs.close(); 
}

void 
writeLog(const string& str)
{
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
}

ostream&
endlog(ostream& os)
{
  Logger* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    writeLog(pLogger->str());
    pLogger->str("");
  }
  else {
    os << endl;
  }

  logtype = LogType_Normal;
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

}
