#include <string>
#include <fstream>
#include <iomanip>

#include "log.hxx"
#include "utils.hxx"

namespace the {

std::string LoadFile(char const *fpath) {
  DEBUG() << "LoadFile(" << std::quoted(fpath) << ')';
  std::string s;
  if (std::ifstream is{fpath, std::ios::binary | std::ios::ate}) {
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
  PANIC() << err.What();
          // << '\n' << err.Where();
  std::terminate();
}

}
