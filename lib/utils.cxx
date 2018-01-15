#include <fstream>
#include <iomanip>

#include "errors.hxx"
#include "utils.hxx"

namespace the {

std::vector<char> LoadFile(char const *fpath) {
  std::vector<char> data;
  if (std::ifstream is{fpath, std::ios::binary | std::ios::ate}) {
    auto size = is.tellg();
    data.resize(size);
    is.seekg(0);
    if (!is.read(data.data(), size))
      Panic(RuntimeError{"could not read file"});
  }
  return std::move(data);
}

}
