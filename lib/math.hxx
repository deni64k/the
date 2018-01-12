#pragma once

#include <cmath>
#include <string>

namespace the {

inline
double FromDMS(int d, int m, double s) {
  // https://en.wikipedia.org/wiki/Ones%27_complement
  // Handle ones' complement platform as two's one comparing integers with zero.
  double sign = (std::abs(d) != 0 && std::signbit(d)) ||
                (std::abs(m) != 0 && std::signbit(m)) ||
                std::signbit(s) ? -1.0 : 1.0;
  return std::copysign(std::abs(d) + std::abs(m) / 60.0 + std::abs(s) / 3600.0, sign);
}

inline
void DMS(double a, int &d, int &m, double &s) {
  double x = std::abs(a);
  d = static_cast<int>(x);

  x -= d;
  x *= 60.0;
  m = static_cast<int>(x);

  x -= m;
  x *= 60.0;
  s = x;

  if (std::signbit(a)) {
    if (std::abs(d) != 0) {
      d = std::copysign(d, -1);
    } else if (std::abs(m) != 0) {
      m = std::copysign(m, -1);
    } else {
      s = std::copysign(s, -1.0);
    }
  }
}

inline
std::string FormatDMS(double a) {
  std::string res;

  int d, m;
  double s;
  DMS(a, d, m, s);

  res += std::to_string(d);
  res += "°";
  res += std::to_string(m);
  res += "'";
  res += std::to_string(s);
  res += "\"";

  return res;
}

inline
std::string FormatHMS(double a) {
  std::string res;

  int d, m;
  double s;
  DMS(a / 15.0, d, m, s);

  res += std::to_string(d);
  res += "h";
  res += std::to_string(m);
  res += "m";
  res += std::to_string(s);
  res += "s";

  return res;
}

/// Calculates cos(alpha + beta) and sin(alpha + beta) using the law of sines.
/// @param c0 cos(alpha)
/// @param s0 sin(alpha)
/// @param c1 cos(beta)
/// @param s1 sin(beta)
/// @param [out] c cos(alpha + beta)
/// @param [out] s sin(alpha + beta)
inline
void SineLaw(double const c0, double const s0,
             double const c1, double const s1,
             double& c, double& s) {
  c = c0 * c1 - s0 * s1;
  s = s0 * c1 + c0 * s1;
}


/// Returns the fractional part of a floating-point number.
inline
double Frac(double const x) {
  return x - std::floor(x);
}

}
