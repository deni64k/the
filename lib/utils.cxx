#include "log.hxx"
#include "utils.hxx"

#include <string>
#include <fstream>
#include <iomanip>

namespace astro {

std::string LoadFile(char const *fpath) {
  std::string s;
  if (std::ifstream is{fpath, std::ios::binary | std::ios::ate}) {
    INFO("reading %s\n", fpath);

    auto size = is.tellg();
    s.resize(size);
    is.seekg(0);
    if (!is.read(s.data(), size))
      throw std::runtime_error("could not read file");
  }
  return std::move(s);
}

}
