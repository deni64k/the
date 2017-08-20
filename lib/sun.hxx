#pragma once

#include "vec.hxx"

namespace astro {

/// Computes the Sun's ecliptical position using analytical series.
/// @param T Time in Julian centuries since J2000
/// @note T in Julian centuries since J2000 (T - MJD_J2000 / 36525)
/// @return Geocentric position of the Sun (in [AU]), referred to the ecliptic and equinox of date.
Vec3 SunPos(double const T);

}
