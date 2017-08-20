#pragma once

namespace astro {
namespace {

// The IAU 2009 System of Astronomical Constants
// http://asa.usno.navy.mil/static/files/2016/Astronomical_Constants_2016.txt

double const kPi   = 3.14159265358979324;
double const kPi2  = kPi * 2.0;
double const kRad  = kPi / 180.0;
double const kDeg  = 180.0 / kPi;
double const kArcs = 3600.0 * 180.0 / kPi;

/// Mean obliquity of the ecliptic, epsilon_0
double const kEpsilonJ2000 = 84381.406;

/// Radius of Earth [km]
double const kR_Earth   =   6378.1366;
/// Radius of Sun [km]
double const kR_Sun     = 696000.0;
/// Radius of Moon [km]
double const kR_Moon    =   1737.4;

/// MJD of Epoch J2000.0
double const kMJD_J2000 = 51544.5;
/// JD of Epoch J2000.0
double const kJD_J2000  = 2451545.0;
/// Epoch J2000.0
double const kT_J2000   =  0.0;
/// Epoch B1950
double const kT_B1950   = -0.500002108;

/// Astronomical unit [km]
double const kAU = 149597870.700;

/// Speed of light [au / day]
double const kC_Light = kAU / 299792.458;

}
}
