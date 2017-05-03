#include <ctime>
#include <iostream>
#include <iomanip>

#include "lib/consts.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"
#include "lib/spheric.hxx"

int main() {
  // Rotterdam's home.
  double const kLat = 51.917337 * astro::kRad;
  double const kLng = 4.474634  * astro::kRad;

  double ra, delta;
  // α Cassiopeiae
  ra = astro::FromDMS(0, 40, 30.4405) * astro::kRad*15.0;
  delta = astro::FromDMS(56, 32, 14.392) * astro::kRad;
  // α Deneb
  // ra = astro::FromDMS(20, 41, 25.91) * astro::kRad*15.0;
  // delta = astro::FromDMS(45, 16, 49.2) * astro::kRad;


  std::time_t now = std::time(nullptr);
  std::tm tm = *std::gmtime(&now);
  // tm.tm_year = 2010 - 1900;
  // tm.tm_mon = 0;
  // tm.tm_mday = 1;
  // tm.tm_hour = 8;
  // tm.tm_min = 0;
  // tm.tm_sec = 0;
  double mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                          tm.tm_hour, tm.tm_min, tm.tm_sec);
  double tau = astro::GMST(mjd) + kLng - ra;

  std::cout << "Latitude: " << astro::FormatDMS(kLat * astro::kDeg) << '\n';
  std::cout << "Altitude: " << astro::FormatDMS(kLng * astro::kDeg) << '\n';
  std::cout << "Time: " << std::put_time(&tm, "%c %Z") << std::endl;
  std::cout << "MJD: " << mjd << std::endl;
  std::cout << "tau: " << tau << std::endl;
  std::cout << "tau: " << astro::FormatDMS(tau * astro::kDeg) << std::endl;
  std::cout << "GMST: " << astro::FormatHMS(tau * astro::kDeg) << std::endl;
  std::cout << '\n';
  std::cout << "Star coordinates:" << std::endl;
  std::cout << "RA: " << astro::FormatHMS(ra * astro::kDeg) << '\t';
  std::cout << "Delta: " << astro::FormatDMS(delta * astro::kDeg) << std::endl;

  double az, elev;
  astro::Equ2Hor(delta, tau, kLat, elev, az);

  std::cout << '\n';
  std::cout << "Current horizontal coordinates:\n";
  std::cout << "Azimuth: " << astro::FormatDMS(az * astro::kDeg) << '\t';
  std::cout << "Altitude: " << astro::FormatDMS(elev * astro::kDeg) << std::endl;

  return 0;
}
