#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace astro {

template <int Size, typename T>
struct Vec final: std::array<T, Size> {
  constexpr Vec() {};
  constexpr Vec(Vec const &) = default;
  template <typename... Ts>
  constexpr Vec(Ts... args)
      : std::array<T, Size>{args...}
  {}
};

using Vec3 = Vec<3, double>;
using Vec3f = Vec<3, float>;

struct Polar {
  double Phi, Theta, R;
};

template <typename T = double>
inline
Vec<3, T> MakeVec3(Polar const &polar) {
  double const thetaC = std::cos(polar.Theta);
  double const thetaS = std::sin(polar.Theta);
  return {
    polar.R * thetaC * std::cos(polar.Phi),
    polar.R * thetaC * std::sin(polar.Phi),
    polar.R * thetaS
  };
}

template <typename T = double>
inline
Polar MakePolar(Vec<3, T> const &vec) {
  auto const xy      = vec[0]*vec[0] + vec[1]*vec[1];
  auto const r       = std::sqrt(xy + vec[2]*vec[2]);
  auto const phi     = vec[0] == 0.0 && vec[1] == 0.0 ? 0.0 : std::atan2(vec[1], vec[0]);
  auto const xy_sqrt = std::sqrt(xy);
  auto const theta   = vec[2] == 0.0 && xy_sqrt == 0.0 ? 0.0 : std::atan2(vec[2], xy_sqrt);
  return {phi, theta, r};
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator + (Vec<Size, T> const &left, Vec<Size, T> const &right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] + right[i];
  }
  return result;
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator - (Vec<Size, T> const &left, Vec<Size, T> const &right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] - right[i];
  }
  return result;
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator + (Vec<Size, T> const &left, T const right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] + right;
  }
  return result;
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator + (T const left, Vec<Size, T> const &right) {
  return operator + (right, left);
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator - (Vec<Size, T> const &left, T const right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] - right;
  }
  return result;
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator - (T const left, Vec<Size, T> const &right) {
  return operator - (right, left);
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator * (Vec<Size, T> const &left, T const right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] * right;
  }
  return result;
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator * (T const left, Vec<Size, T> const &right) {
  return operator * (right, left);
}

template <int Size, typename T>
inline constexpr
Vec<Size, T>
operator / (Vec<Size, T> const &left, T const right) {
  Vec<Size, T> result;
  for (int i = 0; i < Size; ++i) {
    result[i] = left[i] / right;
  }
  return result;
}

// Cross and dot products defined only for 3D vectors.
template <typename T>
inline constexpr
Vec<3, T>
Dot(Vec<3, T> const &left, Vec<3, T> const &right) {
  return {
    left[0] * right[0],
    left[1] * right[1],
    left[2] * right[2]
  };
}

template <typename T>
inline constexpr
Vec<3, T>
Cross(Vec<3, T> const &left, Vec<3, T> const &right) {
  return {
    left[1] * right[2] - left[2] * right[1],
    left[2] * right[0] - left[0] * right[2],
    left[0] * right[1] - left[1] * right[2]
  };
}

template <int Size, typename T>
inline
std::ostream &
operator << (std::ostream& os, Vec<Size, T> const &vec) {
  os << '[';
  if constexpr (Size > 0) {
    os << vec[0];
    for (int i = 1; i < Size; ++i) {
      os << ',' << ' ' << vec[i];
    }
  }
  os << ']';
  return os;
}

}
