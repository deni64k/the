#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

namespace the {

namespace streams {
extern std::ofstream Null;
}

void Log(char const *format, ...);

}

namespace logging::details {
struct LogLine {
  LogLine(std::ostream &os): os_(os) {}
  ~LogLine() { os_ << '\n'; }

  std::ostream & stream() {
    return os_;
  }

 private:
  std::ostream &os_;
};
}

#define PANIC() if (true) logging::details::LogLine(std::cerr).stream() << "PANIC: " << __func__ << ':' << __LINE__ << ": "
#define ERROR() if (true) logging::details::LogLine(std::cerr).stream() << "ERROR: " << __func__ << ':' << __LINE__ << ": "
#define WARN()  if (true) logging::details::LogLine(std::cerr).stream() << "WARN : " << __func__ << ':' << __LINE__ << ": "
#define INFO()  if (true) logging::details::LogLine(std::cerr).stream() << "INFO : " << __func__ << ':' << __LINE__ << ": "
#define DEBUG() if (true) logging::details::LogLine(std::cerr).stream() << "DEBUG: " << __func__ << ':' << __LINE__ << ": "
