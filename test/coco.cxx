#include "gtest/gtest.h"
#include "lib/consts.hxx"
#include "lib/time.hxx"
#include "lib/spheric.hxx"
#include "lib/sun.hxx"
#include "lib/precnut.hxx"

using namespace astro;

TEST(CoordinateTransformationTest, EquinoxComputesCorrectly) {
  Vec3 v0{1.0, 0.0, 0.0}, v1, v2;
  // {MJD(1950, 1, 1, 0, 0, 0.0)};
  double equinox0{(1950.0 - 2000.0) / 100.0};
  // {MJD(2000, 1, 1, 0, 0, 0.0)};
  double equinox{0.0};
  double epoch{(MJD(1989, 1, 1, 0, 0, 0.0) - kMJD_J2000) / 36525.0};

  // Assume the given coordinates v0 are geocentric and equatorial using
  // J1950.0 equinox and measured on 1989/01/01.
  
  // Switch to J2000.0 equinox.
  // v1 = v0 * PrecMatrixEqu((e0 - MJD_J2000) / 36525.0, (e1 - MJD_J2000) / 36525.0);
  v1 = v0;
  v1 = PrecMatrixEqu(equinox0, equinox) * v1;
  ASSERT_DOUBLE_EQ(0.9999257085750282700, v1[0]);
  ASSERT_DOUBLE_EQ(0.0111788912827816240, v1[1]);
  ASSERT_DOUBLE_EQ(0.0048589834755478631, v1[2]);

  v2 = PrecMatrixEqu(equinox, equinox0) * v1;
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);

  // Convert to ecliptical coordinates.
  v0 = v1;
  v1 = Equ2EclMatrix(equinox) * v1;
  ASSERT_DOUBLE_EQ( 0.9999257085750282700, v1[0]);
  ASSERT_DOUBLE_EQ( 0.0120310386164312440, v1[1]);
  ASSERT_DOUBLE_EQ(-0.0019574065847029198, v1[2]);

  v2 = Ecl2EquMatrix(equinox) * v1;
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);
  
  // Convert to heliocentric coordinates.
  v0 = v1;
  v1 = v1 - PrecMatrixEcl(epoch, equinox) * SunPos(epoch);
  ASSERT_DOUBLE_EQ( 0.8172524687532420900, v1[0]);
  ASSERT_DOUBLE_EQ( 0.9782234496142095900, v1[1]);
  ASSERT_DOUBLE_EQ(-0.0019327623148983524, v1[2]);

  v2 = v1 + PrecMatrixEcl(epoch, equinox) * SunPos(epoch);
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);

  // Convert to equatorial coordinates.
  v0 = v1;
  v1 = Ecl2EquMatrix(equinox) * v1;
  ASSERT_DOUBLE_EQ(0.81725246875324209, v1[0]);
  ASSERT_DOUBLE_EQ(0.82392282283283946, v1[1]);
  ASSERT_DOUBLE_EQ(0.52732915239009448, v1[2]);

  v2 = Equ2EclMatrix(equinox) * v1;
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);
  
  // Switch back to J1950.0 equinox.
  v0 = v1;
  v1 = PrecMatrixEqu(equinox, equinox0) * v1;
  ASSERT_DOUBLE_EQ(0.82896458120275940, v1[0]);
  ASSERT_DOUBLE_EQ(0.81472104153251323, v1[1]);
  ASSERT_DOUBLE_EQ(0.52332953119814429, v1[2]);

  v2 = PrecMatrixEqu(equinox0, equinox) * v1;
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);

  // Convert back to geocentric coordinates.
  v0 = v1;
  v1 = v1 + Ecl2EquMatrix(equinox0) * PrecMatrixEcl(epoch, equinox0) * SunPos(epoch);
  ASSERT_DOUBLE_EQ( 0.99984711305515517000, v1[0]);
  ASSERT_DOUBLE_EQ( 0.00010714650338317089, v1[1]);
  ASSERT_DOUBLE_EQ(-0.00021669460929263362, v1[2]);

  v2 = v1 - Ecl2EquMatrix(equinox0) * PrecMatrixEcl(epoch, equinox0) * SunPos(epoch);
  ASSERT_NEAR(v0[0], v2[0], 1e-9);
  ASSERT_NEAR(v0[1], v2[1], 1e-9);
  ASSERT_NEAR(v0[2], v2[2], 1e-9);
}
