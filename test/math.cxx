#include "gtest/gtest.h"
#include "lib/math.hxx"

using namespace the;

TEST(AngleTest, CalculatesFromDMS) {
  ASSERT_DOUBLE_EQ(  0.0,               FromDMS(0, 0, 0.0));
  ASSERT_DOUBLE_EQ(  0.0,               FromDMS(-0, -0, 0.0));
  ASSERT_DOUBLE_EQ( -0.0,               FromDMS(0, 0, -0.0));
  ASSERT_DOUBLE_EQ(  1.0,               FromDMS(1, 0, 0.0));
  ASSERT_DOUBLE_EQ( -1.0,               FromDMS(-1, 0, 0.0));
  ASSERT_DOUBLE_EQ(-45.0,               FromDMS(45, 0, -0.0));
  ASSERT_DOUBLE_EQ(-46.003472222222221, FromDMS(45, -60, 12.5));
}

TEST(AngleTest, CalculatesDMS) {
  int d, m;
  double s;

  DMS(0.0, d, m, s);
  ASSERT_EQ(0, d);
  ASSERT_EQ(0, m);
  ASSERT_DOUBLE_EQ(0.0, s);

  DMS(-0.0, d, m, s);
  ASSERT_EQ(0, d);
  ASSERT_EQ(0, m);
  ASSERT_DOUBLE_EQ(-0.0, s);

  DMS(1.0, d, m, s);
  ASSERT_EQ(1, d);
  ASSERT_EQ(0, m);
  ASSERT_DOUBLE_EQ(0.0, s);

  DMS(-1.0, d, m, s);
  ASSERT_EQ(-1, d);
  ASSERT_EQ( 0, m);
  ASSERT_DOUBLE_EQ(0.0, s);

  DMS(-59.0/60.0, d, m, s);
  ASSERT_EQ(  0, d);
  ASSERT_EQ(-59, m);
  ASSERT_DOUBLE_EQ(0.0, s);

  DMS(-59.0/3600.0, d, m, s);
  ASSERT_EQ(0, d);
  ASSERT_EQ(0, m);
  ASSERT_DOUBLE_EQ(-59.0, s);
}
