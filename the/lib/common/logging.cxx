#include <cstdarg>
#include <cstdio>

#include "logging.hxx"

namespace the {

std::ofstream streams::Null("/dev/null");

void Log(char const *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

}
