#include "gtest/gtest.h"
#include "lib/vec.hxx"

using namespace astro;

TEST(VectorTest, Instantiates) {
  Vec3 v{1.0, 2.0, 3.0};
  ASSERT_DOUBLE_EQ(1.0, v[0]);
  ASSERT_DOUBLE_EQ(2.0, v[1]);
  ASSERT_DOUBLE_EQ(3.0, v[2]);

  auto xyz = MakeVec3(Polar{0.0, 0.0, 1.0});
  ASSERT_DOUBLE_EQ(1.0, xyz[0]);
  ASSERT_DOUBLE_EQ(0.0, xyz[1]);
  ASSERT_DOUBLE_EQ(0.0, xyz[2]);

  auto polar = MakePolar(Vec3{1.0, 0.0, 0.0});
  ASSERT_DOUBLE_EQ(0.0, polar.phi);
  ASSERT_DOUBLE_EQ(0.0, polar.theta);
  ASSERT_DOUBLE_EQ(1.0, polar.r);
}

TEST(VectorTest, ArithmeticsWork) {
  Vec3 x{1.0, 2.0, 3.0};
  Vec3 y{0.1, 0.01, 0.001};

  {
    auto z = x + y;
    ASSERT_DOUBLE_EQ(1.100, z[0]);
    ASSERT_DOUBLE_EQ(2.010, z[1]);
    ASSERT_DOUBLE_EQ(3.001, z[2]);
  }

  {
    auto z = Dot(x, y);
    ASSERT_DOUBLE_EQ(0.100, z[0]);
    ASSERT_DOUBLE_EQ(0.020, z[1]);
    ASSERT_DOUBLE_EQ(0.003, z[2]);
  }
}
