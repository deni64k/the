#include "gtest/gtest.h"
#include "lib/consts.hxx"
#include "lib/mat.hxx"

using namespace astro;

TEST(MatrixTest, Instantiates) {
  Mat3 m{
    1.0, 2.0, 3.0,
    4.0, 5.0, 6.0,
    7.0, 8.0, 9.0,
  };
  ASSERT_DOUBLE_EQ(1.0, m[0][0]);
  ASSERT_DOUBLE_EQ(2.0, m[0][1]);
  ASSERT_DOUBLE_EQ(3.0, m[0][2]);
  ASSERT_DOUBLE_EQ(4.0, m[1][0]);
  ASSERT_DOUBLE_EQ(5.0, m[1][1]);
  ASSERT_DOUBLE_EQ(6.0, m[1][2]);
  ASSERT_DOUBLE_EQ(7.0, m[2][0]);
  ASSERT_DOUBLE_EQ(8.0, m[2][1]);
  ASSERT_DOUBLE_EQ(9.0, m[2][2]);

  m = Mat3::Id();
  ASSERT_DOUBLE_EQ(1.0, m[0][0]);
  ASSERT_DOUBLE_EQ(0.0, m[0][1]);
  ASSERT_DOUBLE_EQ(0.0, m[0][2]);
  ASSERT_DOUBLE_EQ(0.0, m[1][0]);
  ASSERT_DOUBLE_EQ(1.0, m[1][1]);
  ASSERT_DOUBLE_EQ(0.0, m[1][2]);
  ASSERT_DOUBLE_EQ(0.0, m[2][0]);
  ASSERT_DOUBLE_EQ(0.0, m[2][1]);
  ASSERT_DOUBLE_EQ(1.0, m[2][2]);

  m = MakeMat3(42.0);
  ASSERT_DOUBLE_EQ(42.0, m[0][0]);
  ASSERT_DOUBLE_EQ(42.0, m[0][1]);
  ASSERT_DOUBLE_EQ(42.0, m[0][2]);
  ASSERT_DOUBLE_EQ(42.0, m[1][0]);
  ASSERT_DOUBLE_EQ(42.0, m[1][1]);
  ASSERT_DOUBLE_EQ(42.0, m[1][2]);
  ASSERT_DOUBLE_EQ(42.0, m[2][0]);
  ASSERT_DOUBLE_EQ(42.0, m[2][1]);
  ASSERT_DOUBLE_EQ(42.0, m[2][2]);

  m = MakeMat3(
      {1.0, 4.0, 7.0},
      {2.0, 5.0, 8.0},
      {3.0, 6.0, 9.0}
  );
  ASSERT_DOUBLE_EQ(1.0, m[0][0]);
  ASSERT_DOUBLE_EQ(2.0, m[0][1]);
  ASSERT_DOUBLE_EQ(3.0, m[0][2]);
  ASSERT_DOUBLE_EQ(4.0, m[1][0]);
  ASSERT_DOUBLE_EQ(5.0, m[1][1]);
  ASSERT_DOUBLE_EQ(6.0, m[1][2]);
  ASSERT_DOUBLE_EQ(7.0, m[2][0]);
  ASSERT_DOUBLE_EQ(8.0, m[2][1]);
  ASSERT_DOUBLE_EQ(9.0, m[2][2]);
}

TEST(MatrixTest, GettersOfColsAndRowsWork) {
  Mat3 m{
    1.0, 2.0, 3.0,
    4.0, 5.0, 6.0,
    7.0, 8.0, 9.0,
  };

  auto x = m.Row(0);
  ASSERT_DOUBLE_EQ(1.0, x[0]);
  ASSERT_DOUBLE_EQ(2.0, x[1]);
  ASSERT_DOUBLE_EQ(3.0, x[2]);
  x = m.Row(1);
  ASSERT_DOUBLE_EQ(4.0, x[0]);
  ASSERT_DOUBLE_EQ(5.0, x[1]);
  ASSERT_DOUBLE_EQ(6.0, x[2]);
  x = m.Row(2);
  ASSERT_DOUBLE_EQ(7.0, x[0]);
  ASSERT_DOUBLE_EQ(8.0, x[1]);
  ASSERT_DOUBLE_EQ(9.0, x[2]);

  x = m.Col(0);
  ASSERT_DOUBLE_EQ(1.0, x[0]);
  ASSERT_DOUBLE_EQ(4.0, x[1]);
  ASSERT_DOUBLE_EQ(7.0, x[2]);
  x = m.Col(1);
  ASSERT_DOUBLE_EQ(2.0, x[0]);
  ASSERT_DOUBLE_EQ(5.0, x[1]);
  ASSERT_DOUBLE_EQ(8.0, x[2]);
  x = m.Col(2);
  ASSERT_DOUBLE_EQ(3.0, x[0]);
  ASSERT_DOUBLE_EQ(6.0, x[1]);
  ASSERT_DOUBLE_EQ(9.0, x[2]);
}

TEST(MatrixTest, ArithmeticsWork) {
  Mat3 x{
    1.0, 2.0, 3.0,
    4.0, 5.0, 6.0,
    7.0, 8.0, 9.0,
  };
  Mat3 y{
    9.0, 8.0, 7.0,
    6.0, 5.0, 4.0,
    3.0, 2.0, 1.0,
  };

  auto z{x + y};
  ASSERT_DOUBLE_EQ(10.0, z[0][0]);
  ASSERT_DOUBLE_EQ(10.0, z[0][1]);
  ASSERT_DOUBLE_EQ(10.0, z[0][2]);
  ASSERT_DOUBLE_EQ(10.0, z[1][0]);
  ASSERT_DOUBLE_EQ(10.0, z[1][1]);
  ASSERT_DOUBLE_EQ(10.0, z[1][2]);
  ASSERT_DOUBLE_EQ(10.0, z[2][0]);
  ASSERT_DOUBLE_EQ(10.0, z[2][1]);
  ASSERT_DOUBLE_EQ(10.0, z[2][2]);

  z = z - y;
  ASSERT_DOUBLE_EQ(1.0, z[0][0]);
  ASSERT_DOUBLE_EQ(2.0, z[0][1]);
  ASSERT_DOUBLE_EQ(3.0, z[0][2]);
  ASSERT_DOUBLE_EQ(4.0, z[1][0]);
  ASSERT_DOUBLE_EQ(5.0, z[1][1]);
  ASSERT_DOUBLE_EQ(6.0, z[1][2]);
  ASSERT_DOUBLE_EQ(7.0, z[2][0]);
  ASSERT_DOUBLE_EQ(8.0, z[2][1]);
  ASSERT_DOUBLE_EQ(9.0, z[2][2]);

  z = z * 2.0;
  z = 5.0 * z;
  ASSERT_DOUBLE_EQ(10.0, z[0][0]);
  ASSERT_DOUBLE_EQ(20.0, z[0][1]);
  ASSERT_DOUBLE_EQ(30.0, z[0][2]);
  ASSERT_DOUBLE_EQ(40.0, z[1][0]);
  ASSERT_DOUBLE_EQ(50.0, z[1][1]);
  ASSERT_DOUBLE_EQ(60.0, z[1][2]);
  ASSERT_DOUBLE_EQ(70.0, z[2][0]);
  ASSERT_DOUBLE_EQ(80.0, z[2][1]);
  ASSERT_DOUBLE_EQ(90.0, z[2][2]);

  z = z / 10.0;
  ASSERT_DOUBLE_EQ(1.0, z[0][0]);
  ASSERT_DOUBLE_EQ(2.0, z[0][1]);
  ASSERT_DOUBLE_EQ(3.0, z[0][2]);
  ASSERT_DOUBLE_EQ(4.0, z[1][0]);
  ASSERT_DOUBLE_EQ(5.0, z[1][1]);
  ASSERT_DOUBLE_EQ(6.0, z[1][2]);
  ASSERT_DOUBLE_EQ(7.0, z[2][0]);
  ASSERT_DOUBLE_EQ(8.0, z[2][1]);
  ASSERT_DOUBLE_EQ(9.0, z[2][2]);

  z = -Mat3::Id() * z;
  ASSERT_DOUBLE_EQ(-1.0, z[0][0]);
  ASSERT_DOUBLE_EQ(-2.0, z[0][1]);
  ASSERT_DOUBLE_EQ(-3.0, z[0][2]);
  ASSERT_DOUBLE_EQ(-4.0, z[1][0]);
  ASSERT_DOUBLE_EQ(-5.0, z[1][1]);
  ASSERT_DOUBLE_EQ(-6.0, z[1][2]);
  ASSERT_DOUBLE_EQ(-7.0, z[2][0]);
  ASSERT_DOUBLE_EQ(-8.0, z[2][1]);
  ASSERT_DOUBLE_EQ(-9.0, z[2][2]);
}

TEST(MatrixTest, Transposes) {
  Mat3 x{
    1.0, 4.0, 7.0,
    2.0, 5.0, 8.0,
    3.0, 6.0, 9.0,
  };

  auto y{x.Transpose()};
  ASSERT_DOUBLE_EQ(1.0, y[0][0]);
  ASSERT_DOUBLE_EQ(2.0, y[0][1]);
  ASSERT_DOUBLE_EQ(3.0, y[0][2]);
  ASSERT_DOUBLE_EQ(4.0, y[1][0]);
  ASSERT_DOUBLE_EQ(5.0, y[1][1]);
  ASSERT_DOUBLE_EQ(6.0, y[1][2]);
  ASSERT_DOUBLE_EQ(7.0, y[2][0]);
  ASSERT_DOUBLE_EQ(8.0, y[2][1]);
  ASSERT_DOUBLE_EQ(9.0, y[2][2]);
}

TEST(MatrixTest, Rotates) {
  Vec3 x{1.0, 1.0, 1.0}, y;

  y = Mat3::RotateX(kPi) * x;
  ASSERT_DOUBLE_EQ( 1.0, y[0]);
  ASSERT_DOUBLE_EQ(-1.0, y[1]);
  ASSERT_DOUBLE_EQ(-1.0, y[2]);

  y = Mat3::RotateY(kPi) * x;
  ASSERT_DOUBLE_EQ(-1.0, y[0]);
  ASSERT_DOUBLE_EQ( 1.0, y[1]);
  ASSERT_DOUBLE_EQ(-1.0, y[2]);

  y = Mat3::RotateZ(kPi) * x;
  ASSERT_DOUBLE_EQ(-1.0, y[0]);
  ASSERT_DOUBLE_EQ(-1.0, y[1]);
  ASSERT_DOUBLE_EQ( 1.0, y[2]);
}
