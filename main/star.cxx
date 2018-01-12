#include <ctime>
#include <iostream>
#include <iomanip>

#include "lib/consts.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"
#include "lib/spheric.hxx"

int main() {
  // Rotterdam's home.
  double const kLat = 51.917337 * the::kRad;
  double const kLng = 4.474634  * the::kRad;

  double ra, delta;
  // α Cassiopeiae
  ra = the::FromDMS(0, 40, 30.4405) * the::kRad*15.0;
  delta = the::FromDMS(56, 32, 14.392) * the::kRad;
  // α Deneb
  // ra = the::FromDMS(20, 41, 25.91) * the::kRad*15.0;
  // delta = the::FromDMS(45, 16, 49.2) * the::kRad;


  std::time_t now = std::time(nullptr);
  std::tm tm = *std::gmtime(&now);
  // tm.tm_year = 2010 - 1900;
  // tm.tm_mon = 0;
  // tm.tm_mday = 1;
  // tm.tm_hour = 8;
  // tm.tm_min = 0;
  // tm.tm_sec = 0;
  double mjd = the::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                        tm.tm_hour, tm.tm_min, tm.tm_sec);
  double tau = the::GMST(mjd) + kLng - ra;

  std::cout << "Latitude: " << the::FormatDMS(kLat * the::kDeg) << '\n';
  std::cout << "Altitude: " << the::FormatDMS(kLng * the::kDeg) << '\n';
  std::cout << "Time: " << std::put_time(&tm, "%c %Z") << std::endl;
  std::cout << "MJD: " << mjd << std::endl;
  std::cout << "tau: " << tau << std::endl;
  std::cout << "tau: " << the::FormatDMS(tau * the::kDeg) << std::endl;
  std::cout << "GMST: " << the::FormatHMS(tau * the::kDeg) << std::endl;
  std::cout << '\n';
  std::cout << "Star coordinates:" << std::endl;
  std::cout << "RA: " << the::FormatHMS(ra * the::kDeg) << '\t';
  std::cout << "Delta: " << the::FormatDMS(delta * the::kDeg) << std::endl;

  double az, elev;
  the::Equ2Hor(delta, tau, kLat, elev, az);

  std::cout << '\n';
  std::cout << "Current horizontal coordinates:\n";
  std::cout << "Azimuth: " << the::FormatDMS(az * the::kDeg) << '\t';
  std::cout << "Altitude: " << the::FormatDMS(elev * the::kDeg) << std::endl;

  return 0;
}
