#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdint>

#include "lib/common.hxx"

namespace the {

namespace streams {
extern std::ofstream Null;
}

// void Log(char const *format, ...);

}

namespace logging::details {

struct LogRecord {
  constexpr LogRecord(std::ostream &os): os_(os) {}
  ~LogRecord() { os_ << '\n'; }

  inline constexpr std::ostream & stream() const { return os_; }

 private:
  std::ostream &os_;
};

enum Level: std::uint_fast8_t {
  kPanic   = 0,
  kError   = 1,
  kWarning = 2,
  kInfo    = 3,
  kDebug   = 4
};

}

constexpr auto EnabledLogLevel = logging::details::kDebug;

#define PANIC()                                                     \
  if (logging::details::kPanic <= EnabledLogLevel)                  \
    logging::details::LogRecord(std::cerr).stream()                 \
        << "PANIC: " << __FILE__                                    \
        << " [" << PRETTY_FUNCTION << ':' << __LINE__ << "]: "

#define ERROR()                                                     \
  if (logging::details::kError <= EnabledLogLevel)                  \
    logging::details::LogRecord(std::cerr).stream()                 \
        << "ERROR: "

#define WARN()                                                      \
  if (logging::details::kWarning <= EnabledLogLevel)                \
    logging::details::LogRecord(std::cerr).stream()                 \
        << "WARN: "

#define INFO()                                                      \
  if (logging::details::kInfo <= EnabledLogLevel)                   \
    logging::details::LogRecord(std::cerr).stream()                 \
        << "INFO: "

#define DEBUG()                                                     \
  if (logging::details::kDebug <= EnabledLogLevel)                  \
    logging::details::LogRecord(std::cerr).stream()                 \
        << "DEBUG: " << PRETTY_FUNCTION << ':' << __LINE__ << ": "
