#include <string>
#include <fstream>
#include <iomanip>

#include "log.hxx"
#include "utils.hxx"

namespace astro {

std::string LoadFile(char const *fpath) {
  std::string s;
  if (std::ifstream is{fpath, std::ios::binary | std::ios::ate}) {
    DEBUG() << "Reading " << fpath << '\n';

    auto size = is.tellg();
    s.resize(size);
    is.seekg(0);
    if (!is.read(s.data(), size))
      Panic(Error{"could not read file"});
  }
  return std::move(s);
}

[[noreturn]]
void Panic(Error const &err) noexcept {
  PANIC() << err.What() << '\n';
          // << "stacktrace:\n"
          // << err.Where();
  std::terminate();
}

}
