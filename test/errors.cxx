#include <string>
#include <sstream>

#include "gtest/gtest.h"
#include "lib/errors.hxx"

using namespace the;

TEST(ErrorTest, Instantiates) {
  std::string expected{"occurred"};

  RuntimeError err{expected};
  std::ostringstream ss;
  err.What(ss);
  auto const got = ss.str();

  ASSERT_EQ(expected, got);
}
