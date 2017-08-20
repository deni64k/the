#pragma once

#include <array>

#include "vec.hxx"

namespace astro {

struct Mat3 final: std::array<std::array<double, 3>, 3> {
  static Mat3 Id();

  static Mat3 RotateX(double const angle);
  static Mat3 RotateY(double const angle);
  static Mat3 RotateZ(double const angle);

  Mat3 Transpose() const;

  Vec3 Row(unsigned const index) const;
  Vec3 Col(unsigned const index) const;
};

Mat3 MakeMat3(double const init_value);
Mat3 MakeMat3(Vec3 const& x, Vec3 const& y, Vec3 const& z);

std::ostream& operator << (std::ostream& os, Mat3 const& m);

Mat3 operator + (Mat3 const& x, Mat3 const& y);
Mat3 operator - (Mat3 const& x, Mat3 const& y);
Mat3 operator - (Mat3 const& x);

Mat3 operator * (Mat3 const& x, double const y);
Mat3 operator * (double const x, Mat3 const& y);

Mat3 operator / (Mat3 const& x, double const y);
Mat3 operator / (double const x, Mat3 const& y);

Mat3 operator * (Mat3 const& x, Mat3 const& y);
Vec3 operator * (Vec3 const& x, Mat3 const& y);
Vec3 operator * (Mat3 const& x, Vec3 const& y);

}
