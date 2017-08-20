#include <cassert>
#include <cmath>
#include <ostream>

#include "mat.hxx"

namespace astro {

Mat3 Mat3::Id() {
  return {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
  };
}

Mat3 Mat3::Transpose() const {
  auto const& m = *this;
  return {
    m[0][0], m[1][0], m[2][0],
    m[0][1], m[1][1], m[2][1],
    m[0][2], m[1][2], m[2][2],
  };
}

Mat3 Mat3::RotateX(double const angle) {
  double const s = std::sin(angle);
  double const c = std::cos(angle);
  return {
    1.0,  0.0,  0.0,
    0.0,   +c,   +s,
    0.0,   -s,   +c,
  };
}

Mat3 Mat3::RotateY(double const angle) {
  double const s = std::sin(angle);
  double const c = std::cos(angle);
  return {
     +c, 0.0,  -s,
    0.0, 1.0, 0.0,
     +s, 0.0,  +c,
  };
}

Mat3 Mat3::RotateZ(double const angle) {
  double const s = std::sin(angle);
  double const c = std::cos(angle);
  return {
     +c,  +s, 0.0,
     -s,  +c, 0.0,
    0.0, 0.0, 1.0,
  };
}

Vec3 Mat3::Row(unsigned const i) const {
  assert(i < 3 && "only 3 rows in matrix");
  auto const& m = *this;
  return {m[i][0], m[i][1], m[i][2]};
}

Vec3 Mat3::Col(unsigned const i) const {
  assert(i < 3 && "only 3 columns in matrix");
  auto const& m = *this;
  return {m[0][i], m[1][i], m[2][i]};
}

Mat3 MakeMat3(double const x) {
  return {
    x, x, x,
    x, x, x,
    x, x, x,
  };
}

Mat3 MakeMat3(Vec3 const& x, Vec3 const& y, Vec3 const& z) {
  return {
    x[0], y[0], z[0],
    x[1], y[1], z[1],
    x[2], y[2], z[2],
  };
}

std::ostream& operator << (std::ostream& os, Mat3 const& m) {
  for (int i = 0; i < 3; ++i) {
    os << m[i][0] << '\t' << m[i][1] << '\t' << m[i][2] << '\n';
  }
  return os;
}

Mat3 operator + (Mat3 const &x, Mat3 const &y) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      z[i][j] = x[i][j] + y[i][j];
    }
  }
  return z;
}

Mat3 operator - (Mat3 const &x, Mat3 const &y) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      z[i][j] = x[i][j] - y[i][j];
    }
  }
  return z;
}

Mat3 operator - (Mat3 const &x) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      z[i][j] = -x[i][j];
    }
  }
  return z;
}

Mat3 operator * (Mat3 const &x, double const y) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      z[i][j] = x[i][j] * y;
    }
  }
  return z;
}

Mat3 operator * (double const x, Mat3 const &y) {
  return operator * (y, x);
}

Mat3 operator / (Mat3 const &x, double const y) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      z[i][j] = x[i][j] / y;
    }
  }
  return z;
}

Mat3 operator * (Mat3 const &x, Mat3 const& y) {
  Mat3 z;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      double val = 0;
      for (int k = 0; k < 3; ++k) {
        val += x[i][k] * y[k][j];
      }
      z[i][j] = val;
    }
  }
  return z;
}

Vec3 operator * (Vec3 const& x, Mat3 const &y) {
  Vec3 z{0.0, 0.0, 0.0};
  for (int i = 0; i < 3; ++i) {
    z[i] += x[0] * y[0][i];
    z[i] += x[1] * y[1][i];
    z[i] += x[2] * y[2][i];
  }
  return z;
}

Vec3 operator * (Mat3 const& x, Vec3 const& y) {
  Vec3 z{0.0, 0.0, 0.0};
  for (int i = 0; i < 3; ++i) {
    z[i] += x[i][0] * y[0];
    z[i] += x[i][1] * y[1];
    z[i] += x[i][2] * y[2];
  }
  return z;
}

}
