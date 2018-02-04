#include <cmath>

#include "consts.hxx"
#include "time.hxx"
#include "spheric.hxx"

namespace the {

double EclipticObliquity(double const T) {
  // Astronomical Almanac 2010, p. B52.
  return (kEpsilonJ2000 -
          (46.836769 - (1.831e-4 + (2.00340e-3 - (5.76e-7 - 4.34e-8*T)*T)*T)*T)*T
          / 3600.0) * kRad;
}

Mat3 Equ2EclMatrix(double const T) {
  double const eps = EclipticObliquity(T);
  return Mat3::RotateX(eps * kRad);
}

Mat3 Ecl2EquMatrix(double const T) {
  return Equ2EclMatrix(T).Transpose();
}

void Equ2Hor(double dec, double tau, double lat, 
             double& h, double& az) {
  // auto equ = MakeVec3(Polar{tau, dec, 1.0});
  // auto hor = MakePolar(Mat3::RotateY(kPi/2.0 - lat) * equ);
  // az = hor.Phi;
  // h  = hor.Theta;
  // if (az < 0)
  //   az += kPi2;
  // return;

  auto const tauS = std::sin(tau);
  auto const tauC = std::cos(tau);
  auto const latS = std::sin(lat);
  auto const latC = std::cos(lat);
  h  = std::asin(latS * std::sin(dec) + latC * std::cos(dec) * tauC);
  az = std::atan2(-tauS, latC * std::tan(dec) - latS * tauC);
  if (az < 0)
    az += kPi2;
}

void Hor2Equ(double h, double az, double lat, 
             double& dec, double& tau) {
  // auto hor = MakeVec3(Polar{az, h, 1.0});
  // auto equ = MakePolar(Mat3::RotateY(-(kPi/2.0 - lat)) * hor);

  // tau = equ.Phi;
  // dec = equ.Theta;

  auto const azS = std::sin(az);
  auto const azC = std::cos(az);
  auto const latS = std::sin(lat);
  auto const latC = std::cos(lat);
  dec = std::asin(latS * std::sin(h) - latC * std::cos(h) * azC);
  tau = std::atan2(azS, latC * std::tan(h) + latS * azC);
  if (tau < 0)
    tau += kPi2;
}

}
