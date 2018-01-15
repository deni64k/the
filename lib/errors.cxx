#include <ostream>

#include "errors.hxx"
#include "logging.hxx"

namespace the {

Error::~Error() {
}

std::ostream & operator << (std::ostream &os, Error const &err) {
  err.What(os);
  return os;
}

// virtual boost::stacktrace::stacktrace const & RuntimeError::Where() const noexcept {
//   return stacktrace_;
// }

[[noreturn]]
void Panic(Error const &err) noexcept {
  PANIC() << err;
          // << '\n' << err.Where();
  std::terminate();
}

}
