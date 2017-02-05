#include "Logger.hpp"
#include "include/first.hpp"
#include "boost/locale/time_zone.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <ctime>
#include <iostream>
#include <memory>

using namespace std;
namespace bst = boost::posix_time;

namespace SocketServer 
{

static string LOG_DIR = "/var/log/photona/";

Logger logger;
ofstream Logger::ofs_;

void
Logger::openLog(const std::string& logFilename)
{
  if (ofs_.is_open()){
    ofs_.close();
  }
  ofs_.open((LOG_DIR + logFilename).c_str());

  time_t t = time(0);
  string tz = ::localtime(&t)->tm_zone;
  bst::ptime now(bst::second_clock::local_time());
  logger << "log opened " << bst::to_iso_string(now) << " " << tz << endlog;
}

void
Logger::closeLog() 
{
  if (!ofs_.is_open()) {
    return;
  }

  time_t t = time(0);
  string tz = ::localtime(&t)->tm_zone;
  bst::ptime now(bst::second_clock::local_time());
  logger << "log closed " << bst::to_iso_string(now) << " " << tz << endlog;
  ofs_.close(); 
}

void 
Logger::writeLog_(const string& str)
{
  bst::ptime now(bst::microsec_clock::local_time());
  auto t = now.time_of_day();
  ostringstream timestamp;
  timestamp << setfill('0');
  timestamp << setw(2) << t.hours() << ":";
  timestamp << setw(2) << t.minutes() << ":";
  timestamp << setw(2) << t.seconds() << ".";
  timestamp << setw(3) << t.fractional_seconds() / 1000;

  if (ofs_.is_open()) {
    ofs_ << timestamp.str() << " " << str << std::endl;
  }
  else {
    cout << timestamp.str() << " " << str << std::endl;
  }
}

// manipulator to write log msg. (see std::endl)
ostream&
endlog(std::ostream& os)
{
  auto* pLogger = dynamic_cast<Logger*>(&os);
  if (pLogger) {
    pLogger->writeLog_(pLogger->str());
    pLogger->str("");
  }
  else {
    os << std::endl;
  }

  return os;
}

}
