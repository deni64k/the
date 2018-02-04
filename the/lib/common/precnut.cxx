#include "consts.hxx"
#include "precnut.hxx"

namespace the {

// XXX: https://syrte.obspm.fr/iau2006/aa03_412_P03.pdf
// XXX: https://en.wikipedia.org/wiki/Axial_precession
Mat3 PrecMatrixEcl(double const T0, double const T1) {
  double const dT  = T1 - T0;
  double const Pi  = 174.876383889 * kRad + 
                     (((3289.4789 + 0.60622 * T0) * T0) +
                      ((-869.8089 - 0.50491 * T0) + 0.03536 * dT) * dT) / kArcs;
  double const pi  = ((47.0029 - (0.06603 - 0.000598 * T0) * T0) +
                      ((-0.03302 + 0.000598 * T0) + 0.000060 * dT) * dT) * dT / kArcs;
  double const p_a = ((5029.0966 + (2.22226 - 0.000042 * T0) * T0) +
                      ((1.11113 - 0.000042 * T0) - 0.000006 * dT) * dT) * dT / kArcs;
  return Mat3::RotateZ(-(Pi + p_a)) * Mat3::RotateX(pi) * Mat3::RotateZ(Pi);
}

Mat3 PrecMatrixEqu(double const T0, double const T1) {
  double const dT    = T1 - T0;
  double const zeta  = ((2306.2181 + (1.39656 - 0.000139 * T0) * T0) +
                       ((0.30188 - 0.000344 * T0) + 0.017998 * dT) * dT) * dT / kArcs;
  double const z     = zeta + ((0.79280 + 0.000411 * T0) + 0.000205 * dT) * dT * dT / kArcs;
  double const theta = ((2004.3109 - (0.85330 + 0.000217 * T0) * T0) -
                        ((0.42665 + 0.000217 * T0) + 0.041833 * dT) * dT) * dT / kArcs;
  return Mat3::RotateZ(-z) * Mat3::RotateY(theta) * Mat3::RotateZ(-zeta);
}

}
