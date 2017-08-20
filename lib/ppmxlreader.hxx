#pragma once

#include <cstdlib>
#include <istream>

namespace astro {

/// Implements a reader of PPMXL catalogue.
/// The catalogue contains 910468688 records.
struct PPMXLReader {
  PPMXLReader(std::istream &is, std::size_t limit = 0):
      is_(is),
      limit_(limit)
  {}

  /// Implements a row of PPMXL catalogue.
  struct Row {
    // Description of fields:
    // http://dc.zah.uni-heidelberg.de/__system__/dc_tables/show/tableinfo/ppmxl.main?dbOrder=True#serviceref

    /// Identifier (Q3C ipix of the USNO-B 1.0 object)
    /// Example: \a 161387954652791
    std::uint64_t Ipix;
    /// Right Ascension J2000.0, epoch 2000.0
    /// Example: \a 314.709206
    double RaJ2000;
    /// Declination J2000.0, epoch 2000.0
    /// Example: \a 35.640741
    double DecJ2000;
    /// Mean error in RA*cos(delta) at mean epoch
    /// Example: \a 2.31e-05
    double E_eaepRA;
    /// Mean error in Dec at mean epoch
    /// Example: \a 2.31e-05
    double E_deepDE;
    /// Proper Motion in RA*cos(delta)
    /// Example: \a -4.9444e-07
    double PmRA;
    /// Proper Motion in Dec
    /// Example: \a -1.3944e-06
    double PmDE;
    /// Mean error in pmRA*cos(delta)
    /// Example: \a 1.17e-06
    double E_pmRA;
    /// Mean error in pmDE
    /// Example: \a 1.17e-06
    double E_pmDE;
    /// Number of observations used
    /// Example: \a 6
    unsigned NObs;
    /// Mean Epoch (RA)
    /// Example: \a 1984.55
    double EpochRA;
    /// Mean Epoch (Dec)
    /// Example: \a 1984.55
    double EpochDec;
    /// J selected default magnitude from 2MASS
    /// Example: \a 15.634
    double Jmag;
    /// J total magnitude uncertainty
    /// Example: \a 0.047
    double E_Jmag;
    /// H selected default magnitude from 2MASS
    /// Example: \a 15.238
    double Hmag;
    /// H total magnitude uncertainty
    /// Example: \a 0.094
    double E_Hmag;
    /// K_s selected default magnitude from 2MASS
    /// Example: \a 15.264
    double Kmag;
    /// K_s total magnitude uncertainty
    /// Example: \a 0.170
    double E_Kmag;
    /// B mag from USNO-B, first epoch
    /// Example: \a 18.29
    double B1mag;
    /// B mag from USNO-B, second epoch
    /// Example: \a 18.22
    double B2mag;
    /// R mag from USNO-B, first epoch
    /// Example: \a 16.57
    double R1mag;
    /// R mag from USNO-B, second epoch
    /// Example: \a 16.77
    double R2mag;
    /// I mag from USNO-B
    /// Example: \a 16.64
    double Imag;
    /// Surveys the USNO-B magnitudes are taken from
    /// Example: \a 02137
    std::string MagSurveys;
    /// Flags
    /// Example: \a 0
    std::string Flags;
    /// Proper motion in RA as re-corrected according to 2016AJ....151...99V,
    /// cos(delta) applied. This is only available for objects with 2MASS J magnitudes.
    /// e_pmRA still is suitable as an error estimate.
    /// Example: \a -1.3633e-07
    double VickersPMRA;
    /// Proper motion in Dec as re-corrected according to 2016AJ....151...99V.
    /// This is only available for objects with 2MASS J magnitudes.
    /// e_pmDE still is suitable as an error estimate.
    /// Example: \a -1.0106e-07
    double VickersPMDE;

    friend
    std::istream & operator >> (std::istream &is, Row &row) {
      char delim;
      
      is >> row.Ipix >> delim >> row.RaJ2000 >> delim >> row.DecJ2000 >> delim
         >> row.E_eaepRA >> delim >> row.E_deepDE >> delim >> row.PmRA >> delim
         >> row.PmDE >> delim >> row.E_pmRA >> delim >> row.E_pmDE >> delim
         >> row.NObs >> delim >> row.EpochRA >> delim >> row.EpochDec >> delim
         >> row.Jmag >> delim >> row.E_Jmag >> delim
         >> row.Hmag >> delim >> row.E_Hmag >> delim
         >> row.Kmag >> delim >> row.E_Kmag >> delim
         >> row.B1mag >> delim >> row.B2mag >> delim
         >> row.R1mag >> delim >> row.R2mag >> delim
         >> row.Imag;
      // Ignore the rest.
      return is;
    }

    void Fill(std::string s) {
      char const *delim = "|";
      char *tok = std::strtok(s.data(), delim);

      if (!tok)
        return;

      Ipix = std::atoll(tok);
      tok = std::strtok(nullptr, delim);
      RaJ2000 = std::atof(tok);
      tok = std::strtok(nullptr, delim);
      DecJ2000 = std::atof(tok);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      tok = std::strtok(nullptr, delim);
      // std::cerr << "s=" << s << '\n';
      // std::cerr << "tok=" << tok << '\n';
      if (std::strncmp(tok, "None", 4) == 0) {
        Jmag = -1.0;
      } else {
        Jmag = std::atof(tok);
      }
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
      // tok = std::strtok(nullptr, delim);
    }
  };

  friend
  PPMXLReader & operator >> (PPMXLReader &is, Row &row) {
    std::string line;
    std::getline(is.is_, line);

    if (is.limit_ > 0) {
      if (is.pos_ > is.limit_) {
        return is;
      }
      ++is.pos_;
    }

    row.Fill(line);

    return is;
  }

  operator bool () const {
    if (limit_ > 0 && pos_ > limit_)
      return false;
    
    return is_.operator bool();
  }

 private:
  std::istream &is_;
  std::size_t limit_ = 0;
  std::size_t pos_ = 0;
};

}
