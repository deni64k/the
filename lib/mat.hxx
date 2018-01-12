#pragma once

#include <array>
#include <cmath>
#include <ostream>
#include <type_traits>

#include "common.hxx"
#include "vec.hxx"

namespace astro {

template <int Rows, int Cols, typename T = double>
struct Mat final: std::array<std::array<T, Cols>, Rows> {
  constexpr Mat() {};
  constexpr Mat(Mat const &) = default;
  template <typename... Ts>
  constexpr Mat(Ts... args)
      : std::array<std::array<T, Cols>, Rows>{args...}
  {}

  template <int Size>
  constexpr Mat(Vec<Size, T> const &vec) {
    [[maybe_unused]] bool constexpr vert = Cols == 1 && Size == Rows;
    [[maybe_unused]] bool constexpr hor  = Rows == 1 && Size == Cols;
    if constexpr (vert) {
      for (int i = 0; i < Rows; ++i) {
        (*this)[i][0] = vec[i];
      }
    } else if constexpr (hor) {
      for (int i = 0; i < Rows; ++i) {
        (*this)[0][i] = vec[i];
      }
    } else {
      static_assert(!vert && !hor, "wrongly sized vector");
    }
  }
  
  template <int R = Rows, int C = Cols>
  static
  inline constexpr
  std::enable_if_t<R == C, Mat<R, C, T>>
  Id() {
    Mat<R, C, T> result;

    for (int i = 0; i < R; ++i) {
      for (int j = 0; j < C; ++j) {
        result[i][j] = 0;
      }
    }

    for (int i = 0; i < Rows; ++i) {
      result[i][i] = 1;
    }

    return result;
  }

  template <int R = Rows, int C = Cols>
  static
  inline constexpr
  std::enable_if_t<R == 3 && C == 3, Mat<3, 3, T>>
  RotateX(double const angle) {
    double const s = std::sin(angle);
    double const c = std::cos(angle);
    return {
      1.0,  0.0,  0.0,
      0.0,   +c,   +s,
      0.0,   -s,   +c,
    };
  }

  template <int R = Rows, int C = Cols>
  static
  inline constexpr
  std::enable_if_t<R == 3 && C == 3, Mat<3, 3, T>>
  RotateY(double const angle) {
    double const s = std::sin(angle);
    double const c = std::cos(angle);
    return {
       +c, 0.0,  -s,
      0.0, 1.0, 0.0,
       +s, 0.0,  +c,
    };
  }

  template <int R = Rows, int C = Cols>
  static
  inline constexpr
  std::enable_if_t<R == 3 && C == 3, Mat<3, 3, T>>
  RotateZ(double const angle) {
    double const s = std::sin(angle);
    double const c = std::cos(angle);
    return {
       +c,  +s, 0.0,
       -s,  +c, 0.0,
      0.0, 0.0, 1.0,
    };
  }

  inline constexpr
  Mat<Cols, Rows, T>
  Transpose() const {
    Mat<Cols, Rows, T> result;

    for (int i = 0; i < Rows; ++i) {
      for (int j = 0; j < Cols; ++j) {
        result[j][i] = (*this)[i][j];
      }
    }

    return result;
  }

  inline constexpr
  Vec<Cols, T>
  Row(int const index) const {
    Vec<Cols, T> result;

    for (int i = 0; i < Cols; ++i) {
      result[i] = (*this)[index][i];
    }

    return result;
  }
  
  inline constexpr
  Vec<Rows, T>
  Col(int const index) const {
    Vec<Rows, T> result;

    for (int i = 0; i < Rows; ++i) {
      result[i] = (*this)[i][index];
    }

    return result;
  }
};

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator + (Mat<Rows, Cols, T> const &lhs, Mat<Rows, Cols, T> const &rhs) {
  Mat<Rows, Cols, T> result;
  for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
      result[i][j] = lhs[i][j] + rhs[i][j];
    }
  }
  return std::move(result);
}

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator - (Mat<Rows, Cols, T> const &lhs, Mat<Rows, Cols, T> const &rhs) {
  Mat<Rows, Cols, T> result;
  for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
      result[i][j] = lhs[i][j] - rhs[i][j];
    }
  }
  return std::move(result);
}

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator - (Mat<Rows, Cols, T> const &operand) {
  Mat<Rows, Cols, T> result;
  for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
      result[i][j] = -operand[i][j];
    }
  }
  return result;
}

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator * (Mat<Rows, Cols, T> const &lhs, double const rhs) {
  Mat<Rows, Cols, T> result;
  for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
      result[i][j] = lhs[i][j] * rhs;
    }
  }
  return result;
}

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator * (double const lhs, Mat<Rows, Cols, T> const &rhs) {
  return operator * (rhs, lhs);
}

template <int LhsRows, int Rank, int RhsCols, typename T>
inline constexpr
Mat<LhsRows, RhsCols, T>
operator * (Mat<LhsRows, Rank, T> const &x, Mat<Rank, RhsCols, T> const &y) {
  Mat<LhsRows, RhsCols, T> result;
  for (int i = 0; i < LhsRows; ++i) {
    for (int j = 0; j < RhsCols; ++j) {
      T sum = 0;
      for (int k = 0; k < Rank; ++k) {
        sum += x[i][k] * y[k][j];
      }
      result[i][j] = sum;
    }
  }
  return result;
}

template <int Rows, int Cols, typename T>
inline constexpr
Mat<Rows, Cols, T>
operator / (Mat<Rows, Cols, T> const &lhs, double const rhs) {
  Mat<Rows, Cols, T> result;
  for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
      result[i][j] = lhs[i][j] / rhs;
    }
  }
  return result;
}

template <int Rows, int Size, typename T>
inline constexpr
Vec<Size, T>
operator * (Mat<Rows, Size, T> const &lhs, Vec<Size, T> const &rhs) {
  return (lhs * Mat<Size, 1, T>(rhs)).Col(0);
}

template <int Size, int Cols, typename T>
inline constexpr
Vec<Size, T>
operator * (Vec<Size, T> const &lhs, Mat<Size, Cols, T> const &rhs) {
  return (Mat<1, Size, T>(lhs) * rhs).Row(0);
}

using Mat3 = Mat<3, 3, double>;
using Mat4 = Mat<4, 4, double>;

using Mat3f = Mat<3, 3, float>;
using Mat4f = Mat<4, 4, float>;

inline constexpr
Mat3
MakeMat3(double const x) {
  return {
    x, x, x,
    x, x, x,
    x, x, x,
  };
}

inline constexpr
Mat3
MakeMat3(Vec3 const &x, Vec3 const &y, Vec3 const &z) {
  return {
    x[0], y[0], z[0],
    x[1], y[1], z[1],
    x[2], y[2], z[2],
  };
}

template <int Rows, int Cols, typename T>
std::ostream & operator << (std::ostream &os, Mat<Rows, Cols, T> const &m) {
  for (int i = 0; i < Rows; ++i) {
    if constexpr (Cols > 0) {
      os << m[i][0];
    }
    for (int j = 1; j < Cols; ++j) {
      os << '\t' << m[i][j];
    }
    os << '\n';
  }
  return os;
}

}
