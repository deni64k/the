#pragma once

#include <array>

namespace astro {

struct Vec3 final: std::array<double, 3> {
};

struct Polar {
  double Phi, Theta, R;
};

Vec3 MakeVec3(Polar const& polar);
Polar MakePolar(Vec3 const& vec);

Vec3 Dot(Vec3 const& a, Vec3 const& b);

std::ostream& operator << (std::ostream& os, Vec3 const& a);

Vec3 operator + (Vec3 const&a, Vec3 const& b);
Vec3 operator - (Vec3 const& a, Vec3 const& b);

Vec3 operator + (Vec3 const& a, double const k);
Vec3 operator + (double const k, Vec3 const& a);
Vec3 operator - (Vec3 const& a, double const k);
Vec3 operator - (double const k, Vec3 const& a);
Vec3 operator * (Vec3 const& a, double const k);
Vec3 operator / (Vec3 const& a, double const k);
Vec3 operator * (double const k, Vec3 const& a);

}
