#pragma once

#include "mat.hxx"

namespace the {

/// Returns a precession transformation matrix of ecliptic coordinates.
/// @param T0 Epoch given
/// @param T1 Epoch to precess to
/// @note: T0 and T1 in Julian centuries since J2000 (T - MJD_J2000 / 36525)
Mat3 PrecMatrixEcl(double const T0, double const T1);

/// Returns a precession transformation matrix of equatorial coordinates.
/// @param T0 Epoch given
/// @param T1 Epoch to precess to
/// @note: T0 and T1 in Julian centuries since J2000 (T - MJD_J2000 / 36525)
Mat3 PrecMatrixEqu(double const T0, double const T1);

}
