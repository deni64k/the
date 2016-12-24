#include "gtest/gtest.h"
#include "lib/time.hxx"

using namespace astro;

TEST(CalendarTest, CalculatesMJD) {
  ASSERT_DOUBLE_EQ(    0, MJD(1858, 11, 17, 0, 0, 0.0));
  ASSERT_DOUBLE_EQ(51544, MJD(2000,  1,  1, 0, 0, 0.0));
  ASSERT_DOUBLE_EQ(51544.999988425923,  // ~51544d 23h 59m 59.5s
                   MJD(2000, 1, 1, 23, 59, 59.0));
}

TEST(CalendarTest, CalculatesFromMJD) {
  int year, month, day, hour, minutes;
  double seconds;

  FromMJD(0, year, month, day, hour, minutes, seconds);
  ASSERT_EQ(1858, year);
  ASSERT_EQ(  11, month);
  ASSERT_EQ(  17, day);
  ASSERT_EQ(   0, hour);
  ASSERT_EQ(   0, minutes);
  ASSERT_DOUBLE_EQ(0, seconds);

  FromMJD(51544.0, year, month, day, hour, minutes, seconds);
  ASSERT_EQ(2000, year);
  ASSERT_EQ(   1, month);
  ASSERT_EQ(   1, day);
  ASSERT_EQ(   0, hour);
  ASSERT_EQ(   0, minutes);
  ASSERT_DOUBLE_EQ(0, seconds);

  FromMJD(51544.0 + 23.0/24.0 + 59.0/24.0/60.0 + 59.5/24.0/60.0/60.0,
          year, month, day, hour, minutes, seconds);
  ASSERT_EQ(2000, year);
  ASSERT_EQ(   1, month);
  ASSERT_EQ(   1, day);
  ASSERT_EQ(   23, hour);
  ASSERT_EQ(   59, minutes);
  ASSERT_DOUBLE_EQ(59.500000197440386, seconds);
}
