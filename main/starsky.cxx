#include <cassert>
#include <cstring>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "lib/consts.hxx"
#include "lib/graphics.hxx"
#include "lib/log.hxx"
#include "lib/ppmxlreader.hxx"
#include "lib/spheric.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"

namespace chrono = std::chrono;

using astro::Graphics;

void PrintGraphicsInfo() {
  GLubyte const *renderer = glGetString(GL_RENDERER); // get renderer string
  GLubyte const *version = glGetString(GL_VERSION); // version as a string
  INFO("Renderer: %s\n", renderer);
  INFO("OpenGL version supported: %s\n", version);

  GLfloat sizes[2];  // stores supported point size range
  GLfloat step;      // stores supported point size increments
  glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
  glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
  INFO("Supported point size range: %f ... %f\n", sizes[0], sizes[1]);
  INFO("Supported point size increments: %f\n", step);

  GLenum params[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO,
  };
  const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO",
  };
  INFO("GL Context Params:\n");
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    INFO("%s: %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  INFO("%s: %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  INFO("%s: %u\n", names[11], (unsigned int)s);
  INFO("-----------------------------\n");
}

using astro::PPMXLReader;

struct Starsky {
  Starsky()
  {}

  void Init() {
    time_ = chrono::steady_clock::now();

    auto time = std::time_t(chrono::duration_cast<chrono::seconds>(time_.time_since_epoch()).count());
    std::tm tm = *std::gmtime(&time);
    using milliseconds_type = std::chrono::duration<double, std::chrono::milliseconds::period>;
    auto secs = tm.tm_sec + chrono::duration_cast<milliseconds_type>(time_.time_since_epoch()).count();
    secs = std::fmod(secs, 1.0);
    double mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                            tm.tm_hour, tm.tm_min, secs);
    double gmst = astro::GMST(mjd) + positionLongitude_;

    std::cerr << "Latitude: " << astro::FormatDMS(positionLatitude_ * astro::kDeg) << '\n';
    std::cerr << "Altitude: " << astro::FormatDMS(positionLongitude_ * astro::kDeg) << '\n';
    std::cerr << "Time: " << std::put_time(&tm, "%c %Z") << '\n';
    std::cerr << "MJD: " << mjd << '\n';
    std::cerr << "GMST: " << astro::FormatHMS(gmst * astro::kDeg) << '\n';
    std::cerr << '\n';
  }

  void LoadStars(std::istream &is) {
    PPMXLReader reader(is);
    PPMXLReader::Row data;
    while (reader >> data) {
      entries_.push_back(data);
    }
    INFO("%d stars loaded from the catalogue\n", entries_.size());
  }

  void SetTime(chrono::steady_clock::time_point const &time) {
    time_ = time;
  }

  void SetRotation(double viewAngleX, double viewAngleY) {
    viewAngleX_ = viewAngleX;
    viewAngleY_ = viewAngleY;
  }

  void ToggleExtra() {
    showExtra_ = !showExtra_;
  }

  void VertexizeStars() {
    Reset();

    auto time = std::time_t(chrono::duration_cast<chrono::seconds>(time_.time_since_epoch()).count());
    std::tm tm = *std::gmtime(&time);
    using milliseconds_type = std::chrono::duration<double, std::chrono::milliseconds::period>;
    auto secs = tm.tm_sec + chrono::duration_cast<milliseconds_type>(time_.time_since_epoch()).count();
    secs = std::fmod(secs, 1.0);
    double const mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                  tm.tm_hour, tm.tm_min, secs);
    double const gmst = astro::GMST(mjd) + positionLongitude_;

    for (auto const &data : entries_) {
      // Filter out stars invisible for naked eye.
      // if (data.Jmag <= 0.0 || data.Jmag >= 2.5)
      //   continue;

      double ra, delta;
      ra    = data.RaJ2000 * astro::kRad;
      delta = data.DecJ2000 * astro::kRad;
      // DEBUG("data: ipix=%llu, ra=%lf (%lf), delta=%lf (%lf)\n",
      //       data.Ipix, data.RaJ2000, ra, data.DecJ2000, delta);

      ProcessStar(gmst - ra, delta, data.Jmag / (2.5 / 10.0) + 5.0);

      // double const tau = gmst - ra;

      // std::cerr << "Star #" << data.Ipix << '\n';
      // std::cerr << "RA:       " << astro::FormatHMS(ra * astro::kDeg) << '\t';
      // std::cerr << "Delta:    " << astro::FormatDMS(delta * astro::kDeg) << '\n';

      // double az, elev;
      // astro::Equ2Hor(delta, tau, positionLatitude_, elev, az);

      // std::cerr << "Azimuth:  " << astro::FormatDMS(az * astro::kDeg) << '\t';
      // std::cerr << "Altitude: " << astro::FormatDMS(elev * astro::kDeg) << '\n';
      // std::cerr << '\n';

      // int x = std::cos(elev) * std::cos(az) * Graphics::WindowWidth()/2;
      // int y = std::sin(elev) * Graphics::WindowHeight()/2;
      // std::cerr << "x: " << float{x}/Graphics::WindowWidth()*2.0 << '\t';
      // std::cerr << "y: " << y << '\n';
      // std::cerr << '\n';
    }

    if (!showExtra_)
      return;

    // North and South poles.
    ProcessStar(gmst,  astro::kPi/2.0, 40);
    // ProcessStar(0.0, -astro::kPi/2.0, gmst, 40);

    // Deneb
    // ProcessStar(310.357978889, 45.2803369444, 5);

    // Orion's Belt
    // ProcessStar(astro::FromDMS(5, 32,  0.40009) * 15.0, astro::FromDMS( 0, -17, 56.7424));
    // ProcessStar(astro::FromDMS(5, 36, 12.8)     * 15.0, astro::FromDMS(-1,  12,  6.9), 5);
    // ProcessStar(astro::FromDMS(5, 40, 45.52666) * 15.0, astro::FromDMS(-1,  56, 34.2649));

    // Cassiopeiea
    ProcessStar(gmst - astro::FromDMS(0, 40, 30.4405)  * 15.0 * astro::kRad, astro::FromDMS(56, 32, 14.3920) * astro::kRad, 30);
    ProcessStar(gmst - astro::FromDMS(0,  9, 10.68518) * 15.0 * astro::kRad, astro::FromDMS(59,  8, 59.2120) * astro::kRad, 30);
    ProcessStar(gmst - astro::FromDMS(0, 56, 42.50108) * 15.0 * astro::kRad, astro::FromDMS(60, 43,  0.2984) * astro::kRad, 30);
    ProcessStar(gmst - astro::FromDMS(1, 25, 48.95147) * 15.0 * astro::kRad, astro::FromDMS(60, 14,  7.0225) * astro::kRad, 30);
    ProcessStar(gmst - astro::FromDMS(1, 54, 23.72567) * 15.0 * astro::kRad, astro::FromDMS(63, 40, 12.3628) * astro::kRad, 30);

    // Sun
    {
      double epoch = (mjd - astro::kMJD_J2000) / 36525.0;
      auto vec = astro::SunPos(epoch);
      // std::cerr << "ecl sun = " << vec << '\n';
      vec = astro::Ecl2EquMatrix(epoch) * vec;
      // std::cerr << "equ sun = " << vec << '\n';
      // The z axis is directed towards to the north celestial polar.
      // To align with the model axis it has to be rotated relatively x on 90 degrees.
      // vec = astro::Mat3::RotateX(-astro::kPi/2.0) * vec;
      // The y axis has got to be reversed direction.
      // vec[2] = -vec[2];
      auto sun = astro::MakePolar(vec);
      // std::cerr << "polar sun = " << vec << '\n';
      // ProcessStar(sun.Phi, sun.Theta, 75);
      double elev, az;
      astro::Equ2Hor(sun.Theta, gmst - sun.Phi, positionLatitude_, elev, az);
      // std::cerr << "elev = " << elev << "; az = " << az << '\n';
      ProcessStar(az, elev, 75);
    }
  }

  void ProcessStar(double ha, double decl, double mag) {
    // double az, elev;
    // astro::Equ2Hor(decl, ha, positionLatitude_, elev, az);

    // DrawStar(az, elev, mag);

    DrawStar(ha, decl, mag);
  }

  void DrawStar(double az, double elev, double mag) {
    float x = std::sin(az) * std::cos(elev);
    float y = std::sin(elev);
    float z = std::cos(az) * std::cos(elev);

    // float x = std::sin(ra) * std::cos(decl);
    // float y = std::sin(decl);
    // float z = std::cos(ra) * std::cos(decl);

    auto vec = astro::Vec3{x, y, z};
    vec = astro::Mat3::RotateX(viewAngleX_) * vec;
    vec = astro::Mat3::RotateY(viewAngleY_) * vec;
    x = vec[0];
    y = vec[1];
    z = vec[2];

    // if (y < 0)
    //   return;

    // float d = x*x + y*y + z*z;

    // DEBUG("vertexing a star: x=% .08f, y=% .08f, z=% .08f, mag=%2i, d=%f\n", x, y, z, mag, d);

    Graphics::Star v{x, y, z, float(double(mag/3.15))};
    stars_.emplace_back(v);
  }

  void Reset() {
    stars_.clear();
  }

  std::vector<Graphics::Star> const & Stars() const {
    return stars_;
  }

 protected:
  // Dublin's home.
  double const positionLatitude_  = 53.319927 * astro::kRad;
  double const positionLongitude_ = -6.264353 * astro::kRad;

  chrono::steady_clock::time_point time_;
  bool showExtra_ = true;

  // Look at the North polar star.
  double viewAngleX_ = 0.0;
  double viewAngleY_ = 0.0;

  std::vector<Graphics::Star> stars_;
  std::vector<PPMXLReader::Row> entries_;
};

struct GraphicsProgram: public Graphics {
  GraphicsProgram()
      : Graphics() {
    Graphics::windowWidth_ = 520;
    Graphics::windowHeight_ = 520;

    timeBeginning_ = chrono::steady_clock::now();
  }

  void Render() override {
    double const timeScale = 600.0;

    chrono::steady_clock::time_point now = chrono::steady_clock::now();
    now += chrono::duration_cast<chrono::nanoseconds>((now - timeBeginning_) * timeScale);

    sky_->SetTime(now);
    sky_->SetRotation(viewAngleX_, viewAngleY_);
    sky_->VertexizeStars();

    LoadStars(sky_->Stars());

    RenderStars();
  }

  void HandleInput() override {
    double const rotationStep = astro::kPi / 12.0 / 60.0;
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_ESCAPE) || GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_Q)) {
      glfwSetWindowShouldClose(window_, 1);
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_LEFT)) {
      viewAngleY_ +=  rotationStep;
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_RIGHT)) {
      viewAngleY_ += -rotationStep;
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_UP)) {
      viewAngleX_ += -rotationStep;
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_DOWN)) {
      viewAngleX_ +=  rotationStep;
    }
    viewAngleX_ = std::remainder(viewAngleX_, astro::kPi2);
    viewAngleY_ = std::remainder(viewAngleY_, astro::kPi2);
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_Z)) {
      viewAngleX_ = -0;//astro::kPi/2.0;
      viewAngleY_ =  0;//astro::kPi;
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_X)) {
      sky_->ToggleExtra();
    }
  }

  void SetSky(Starsky *sky) {
    sky_ = sky;
  }

 private:
  double viewAngleX_ = -(90.0 - 53.319927) * astro::kRad; // astro::kPi/2.0;
  double viewAngleY_ =  0; // astro::kPi;

  Starsky *sky_;
  chrono::steady_clock::time_point timeBeginning_;
};

int main() {
  // std::cin.sync_with_stdio(false);
  // std::cin.tie(nullptr);

  GraphicsProgram graphics;

  try {
    graphics.Init();
    PrintGraphicsInfo();

    gsl::owner<Starsky *> sky = new Starsky{};
    sky->Init();
    sky->LoadStars(std::cin);

    graphics.SetSky(sky);

    graphics.LoadShaders();
    graphics.Loop();
    graphics.Deinit();
  } catch (std::exception const &ex) {
    fprintf(stderr, "ERROR: %s\n", ex.what());
    graphics.Deinit();
    return 1;
  }

  return 0;
}
