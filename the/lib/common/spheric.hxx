#pragma once

#include <array>

#include "mat.hxx"

namespace the {

double EclipticObliquity(double const T);

/// Returns a matrix for transformation of equatorial to ecliptical coordinates.
/// @param T is a time in Julian centuries since J2000
/// @note: T in Julian centuries since J2000 (T - MJD_J2000 / 36525)
Mat3 Equ2EclMatrix(double const T); 

/// Returns a matrix for transformation of ecliptical to equatorial coordinates.
/// @param T is a time in Julian centuries since J2000
/// @note: T in Julian centuries since J2000 (T - MJD_J2000 / 36525)
Mat3 Ecl2EquMatrix(double const T);

// Transforms equatorial coordinates to the horizon system.
// @param dec Declination
// @param tau Hour angle
// @param lat Geographical latitude of the observer
// @param [out] h Altitude
// @param [out] az Azimuth
// @note All parameters in radians.
void Equ2Hor(double dec, double tau, double lat, 
             double& h, double& az);

// Transforms equatorial coordinates to the horizon system.
// @param h Altitude
// @param az Azimuth
// @param lat Geographical latitude of the observer
// @param [out] dec Declination
// @param [out] tau Hour angle
// @note All parameters in radians.
void Hor2Equ(double h, double az, double lat, 
             double& dec, double& tau);

}
