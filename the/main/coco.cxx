#include <ctime>
#include <iostream>
#include <iomanip>

#include "lib/consts.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"

int main() {
  std::time_t const now = std::time(nullptr);
  std::tm tm = *std::gmtime(&now);
  double const epoch = the::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                tm.tm_hour, tm.tm_min, tm.tm_sec);

  std::cout << "Time: " << std::put_time(&tm, "%c %Z") << std::endl;
  std::cout << "Epoch: " << epoch << std::endl;

  the::Vec3 const sun = the::SunPos((epoch - the::kMJD_J2000) / 36525.0);
  std::cout << "Sun coordinates:" << std::endl;
  std::cout << sun << std::endl;

  the::Polar sunPolar = MakePolar(sun);
  // int d, m;
  // double s;
  // DMS(
  std::cout << "         o  '  \"             o  '  \"" << std::endl;
  std::cout << "  L = " << std::setprecision(2) << std::setw(12)
            << the::kDeg * sunPolar.Phi;
  std::cout << "    B = " << std::setprecision(1) << std::showpos << std::setw(11) 
            << the::kDeg * sunPolar.Theta << std::noshowpos;

  // Equator
  // std::cout << "         h  m  s               o  '  \"" << std::endl;
  // std::cout << "  RA = " << std::setprecision(2) << std::setw(11)
  // << Angle(the::kDeg*m_R[phi]/15.0,DMMSSs);
  // std::cout << "    Dec = " << std::setprecision(1) << std::showpos << std::setw(11) 
  // << Angle(the::kDeg*m_R[theta],DMMSSs) << std::noshowpos;

  std::cout << "    R = " << std::setprecision(8) << std::setw(12) 
            << sunPolar.R << std::endl;

  return 0;
}
