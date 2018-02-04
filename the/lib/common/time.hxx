#pragma once

#include <cmath>

#include "consts.hxx"
#include "math.hxx"

namespace the {

inline
double MJD(int year, int month, int day,
           int hour, int minute, double seconds) {
  if (month <= 2) {
    month += 12;
    --year;
  }

  int b;
  if ((10000L * year + 100L * month + day) <= 15821004L) {
    // Julian calendar
    b = -2 + ((year + 4716) / 4) - 1179;
  } else {
    // Gregorian calendar
    b = year/400 - year/100 + year/4;
  }
  double mjd;
  mjd  = 365L * year - 679004L + b + int(30.6001 * (month+1)) + day;
  mjd += FromDMS(hour, minute, seconds) / 24.0;

  return mjd;
}

inline
void FromMJD(double mjd,
             int& year, int& month, int& day, double& hour) {
  long   a, b, c, d, e, f;
  double frac_of_day;

  // Convert Julian day number to calendar date
  a = static_cast<long>(mjd + 2400001.0);

  if (a < 2299161) {  // Julian calendar
    b = 0;
    c = a + 1524;
  } else {            // Gregorian calendar
    b = static_cast<long>((a - 1867216.25) / 36524.25);
    c = a + b - b/4 + 1525;
  }

  d = static_cast<long>((c - 122.1) / 365.25);
  e = 365*d + d/4;
  f = static_cast<long>((c - e) / 30.6001);

  day   = c - e - static_cast<int>(30.6001 * f);
  month = f - 1 - 12*(f/14);
  year  = d - 4715 - (7 + month)/10;

  frac_of_day = mjd - std::floor(mjd);

  hour = 24.0 * frac_of_day;
}

inline
void FromMJD(double mjd,
             int& year, int& month, int& day,
             int& hour, int& minutes, double& seconds) {
  FromMJD(mjd, year, month, day, seconds);

  hour = static_cast<int>(seconds);
  seconds -= hour;
  seconds *= 60.0;
  minutes = static_cast<int>(seconds);
  seconds -= minutes;
  seconds *= 60.0;
}

// Returns Greenwich mean sidereal time.
// @param MJD Time as Modified Julian Date
// @return GMST in [rad]
inline
double GMST(double mjd) {
  // Astronomical Algorithms by Jeen Meeus
  // Chapter 12
  // Sidereal Time at Greenwich
  mjd -= kMJD_J2000;
  double T = mjd / 36525.0;
  double theta0 = 280.46061837 +
                  std::remainder(360.98564736629 * mjd, 360.0) +
                  std::remainder(0.000387933 * T*T, 360.0) -
                  std::remainder(T*T*T / 38710000.0, 360.0);
  theta0 = std::remainder(theta0, 360.0) * kRad;
  if (theta0 < 0)
    theta0 += kPi2;
  return theta0;

  // double const kSecs = 86400.0;        // Seconds per day

  // double mjd_0, UT, T_0, T, gmst;
  // mjd_0 = std::floor(mjd);
  // UT    = kSecs * (mjd - mjd_0);     // [s]
  // T_0   = (mjd_0 - 51544.5) / 36525.0; 
  // T     = (mjd   - 51544.5) / 36525.0; 

  // gmst  = 24110.54841 + 8640184.812866*T_0 + 1.0027379093*UT 
  //         + (0.093104 - 6.2e-6*T)*T*T;      // [sec]

  // return (kPi2 / kSecs) * std::remainder(gmst, kSecs);   // [Rad]  
}

}
