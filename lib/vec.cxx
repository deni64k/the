#include <cmath>
#include <ostream>

#include "vec.hxx"

namespace astro {

Vec3 MakeVec3(Polar const& polar) {
  double const thetaC = std::cos(polar.Theta);
  double const thetaS = std::sin(polar.Theta);
  return {
    polar.R * thetaC * std::cos(polar.Phi),
    polar.R * thetaC * std::sin(polar.Phi),
    polar.R * thetaS
  };
}

Polar MakePolar(Vec3 const& vec) {
  auto const xy      = vec[0]*vec[0] + vec[1]*vec[1];
  auto const r       = std::sqrt(xy + vec[2]*vec[2]);
  auto const phi     = vec[0] == 0.0 && vec[1] == 0.0 ? 0.0 : std::atan2(vec[1], vec[0]);
  auto const xy_sqrt = std::sqrt(xy);
  auto const theta   = vec[2] == 0.0 && xy_sqrt == 0.0 ? 0.0 : std::atan2(vec[2], xy_sqrt);
  return {phi, theta, r};
}

Vec3 Dot(Vec3 const& a, Vec3 const& b) {
  return {a[0] * b[0], a[1] * b[1], a[2] * b[2]};
}

std::ostream& operator << (std::ostream& os, Vec3 const& a) {
  os << '[' << a[0] << ',' << ' ' << a[1] << ',' << ' ' << a[2] << ']';
  return os;
}

Vec3 operator + (Vec3 const&a, Vec3 const& b) {
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

Vec3 operator - (Vec3 const& a, Vec3 const& b) {
  return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

Vec3 operator + (Vec3 const& a, double const k) {
  return {a[0] + k, a[1] + k, a[2] + k};
}

Vec3 operator + (double const k, Vec3 const& a) {
  return operator + (a, k);
}

Vec3 operator - (Vec3 const& a, double const k) {
  return {a[0] - k, a[1] - k, a[2] - k};
}

Vec3 operator - (double const k, Vec3 const& a) {
  return operator - (a, k);
}

Vec3 operator * (Vec3 const& a, double const k) {
  return {a[0] * k, a[1] * k, a[2] * k};
}

Vec3 operator / (Vec3 const& a, double const k) {
  return {a[0] / k, a[1] / k, a[2] / k};
}

Vec3 operator * (double const k, Vec3 const& a) {
  return operator * (a, k);
}

}
