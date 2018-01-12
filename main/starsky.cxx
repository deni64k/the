#include <cassert>
#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

#include "lib/consts.hxx"
#include "lib/log.hxx"
#include "lib/ppmxlreader.hxx"
#include "lib/spheric.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"
#include "lib/ui/fonts.hxx"
#include "lib/ui/graphics.hxx"
#include "lib/ui/impl/errors.hxx"
#include "lib/ui/shader.hxx"
#include "lib/utils.hxx"

namespace chrono = std::chrono;

using the::ui::Graphics;
using the::Fallible;

void PrintGraphicsInfo() {
  INFO() << "Using GLEW "      << glewGetString(GLEW_VERSION) << '\n';
  INFO() << "Vendor: "         << glGetString(GL_VENDOR) << '\n';
  INFO() << "Renderer: "       << glGetString(GL_RENDERER) << '\n';
  INFO() << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
  INFO() << "GLSL version: "   << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

  GLfloat sizes[2];  // stores supported point size range
  GLfloat step;      // stores supported point size increments
  glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
  glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
  INFO() << "Supported point size range: " << sizes[0] << " ... " << sizes[1] << '\n';
  INFO() << "Supported point size increments: " << step << '\n';

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
  const char *names[] = {
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
  INFO() << "GL Context Params:\n";
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    INFO() << names[i] << ": " << v << '\n';
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  INFO() << names[10] << ": " << v[0] << ' ' << v[1] << '\n';
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  INFO() << names[11] << ": " << (unsigned int)s << '\n';
  INFO() << "-----------------------------\n";
  (void)glGetError();  // reset error status
}

using the::PPMXLReader;

struct Starsky {
  Starsky()
  {}

  void Init() {
    time_ = chrono::system_clock::now();

    auto time = std::time_t(chrono::duration_cast<chrono::seconds>(time_.time_since_epoch()).count());
    std::tm tm = *std::gmtime(&time);
    using milliseconds_type = std::chrono::duration<double, std::chrono::milliseconds::period>;
    auto secs = tm.tm_sec + chrono::duration_cast<milliseconds_type>(time_.time_since_epoch()).count();
    secs = std::fmod(secs, 1.0);
    double mjd = the::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                          tm.tm_hour, tm.tm_min, secs);
    double gmst = the::GMST(mjd) + positionLongitude_;

    INFO() << "Initialize with these parameters\n";
    INFO() << "Latitude: " << the::FormatDMS(positionLatitude_ * the::kDeg) << '\n';
    INFO() << "Altitude: " << the::FormatDMS(positionLongitude_ * the::kDeg) << '\n';
    INFO() << "Time: " << std::put_time(&tm, "%c %Z") << '\n';
    INFO() << "MJD: " << mjd << '\n';
    INFO() << "GMST: " << the::FormatHMS(gmst * the::kDeg) << '\n';
    INFO() << "-----------------------------\n";
  }

  void LoadStars(std::istream &is) {
    PPMXLReader reader(is);
    PPMXLReader::Row data;
    while (reader >> data) {
      entries_.push_back(data);
    }
    INFO() << entries_.size() << " stars loaded from the catalogue\n";
  }

  void SetTime(chrono::system_clock::time_point const &time) {
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
    double const mjd = the::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                tm.tm_hour, tm.tm_min, secs);
    double const gmst = the::GMST(mjd) + positionLongitude_;

    for (auto const &data : entries_) {
      // Filter out stars invisible for naked eye.
      // if (data.Jmag <= 0.0 || data.Jmag >= 2.5)
      //   continue;

      double ra, delta;
      ra    = data.RaJ2000 * the::kRad;
      delta = data.DecJ2000 * the::kRad;
      // DEBUG("data: ipix=%llu, ra=%lf (%lf), delta=%lf (%lf)\n",
      //       data.Ipix, data.RaJ2000, ra, data.DecJ2000, delta);

      ProcessStar(gmst - ra, delta, data.Jmag / (2.5 / 10.0) + 5.0);

      // double const tau = gmst - ra;

      // std::cerr << "Star #" << data.Ipix << '\n';
      // std::cerr << "RA:       " << the::FormatHMS(ra * the::kDeg) << '\t';
      // std::cerr << "Delta:    " << the::FormatDMS(delta * the::kDeg) << '\n';

      // double az, elev;
      // the::Equ2Hor(delta, tau, positionLatitude_, elev, az);

      // std::cerr << "Azimuth:  " << the::FormatDMS(az * the::kDeg) << '\t';
      // std::cerr << "Altitude: " << the::FormatDMS(elev * the::kDeg) << '\n';
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
    ProcessStar(gmst,  the::kPi/2.0, 40);
    // ProcessStar(0.0, -the::kPi/2.0, gmst, 40);

    // Deneb
    // ProcessStar(310.357978889, 45.2803369444, 5);

    // Orion's Belt
    // ProcessStar(the::FromDMS(5, 32,  0.40009) * 15.0, the::FromDMS( 0, -17, 56.7424));
    // ProcessStar(the::FromDMS(5, 36, 12.8)     * 15.0, the::FromDMS(-1,  12,  6.9), 5);
    // ProcessStar(the::FromDMS(5, 40, 45.52666) * 15.0, the::FromDMS(-1,  56, 34.2649));

    // Cassiopeiea
    ProcessStar(gmst - the::FromDMS(0, 40, 30.4405)  * 15.0 * the::kRad, the::FromDMS(56, 32, 14.3920) * the::kRad, 30);
    ProcessStar(gmst - the::FromDMS(0,  9, 10.68518) * 15.0 * the::kRad, the::FromDMS(59,  8, 59.2120) * the::kRad, 30);
    ProcessStar(gmst - the::FromDMS(0, 56, 42.50108) * 15.0 * the::kRad, the::FromDMS(60, 43,  0.2984) * the::kRad, 30);
    ProcessStar(gmst - the::FromDMS(1, 25, 48.95147) * 15.0 * the::kRad, the::FromDMS(60, 14,  7.0225) * the::kRad, 30);
    ProcessStar(gmst - the::FromDMS(1, 54, 23.72567) * 15.0 * the::kRad, the::FromDMS(63, 40, 12.3628) * the::kRad, 30);

    // Sun
    {
      double epoch = (mjd - the::kMJD_J2000) / 36525.0;
      auto vec = the::SunPos(epoch);
      // std::cerr << "ecl sun = " << vec << '\n';
      vec = the::Ecl2EquMatrix(epoch) * vec;
      // std::cerr << "equ sun = " << vec << '\n';
      // The z axis is directed towards to the north celestial polar.
      // To align with the model axis it has to be rotated relatively x on 90 degrees.
      // vec = the::Mat3::RotateX(-the::kPi/2.0) * vec;
      // The y axis has got to be reversed direction.
      // vec[2] = -vec[2];
      auto sun = the::MakePolar(vec);
      // std::cerr << "polar sun = " << vec << '\n';
      // ProcessStar(sun.Phi, sun.Theta, 75);
      double elev, az;
      the::Equ2Hor(sun.Theta, gmst - sun.Phi, positionLatitude_, elev, az);
      // std::cerr << "elev = " << elev << "; az = " << az << '\n';
      ProcessStar(az, elev, 75);
    }
  }

  void ProcessStar(double ha, double decl, double mag) {
    // double az, elev;
    // the::Equ2Hor(decl, ha, positionLatitude_, elev, az);

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

    auto vec = the::Vec3{x, y, z};
    vec = the::Mat3::RotateX(viewAngleX_) * vec;
    vec = the::Mat3::RotateY(viewAngleY_) * vec;
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
  double const positionLatitude_  = 53.319927 * the::kRad;
  double const positionLongitude_ = -6.264353 * the::kRad;

  // TODO: Switch to real clock and increment gradually with timeFactor.
  chrono::system_clock::time_point time_;
  bool showExtra_ = true;

  // Look at the North polar star.
  double viewAngleX_ = 0.0;
  double viewAngleY_ = 0.0;

  std::vector<Graphics::Star> stars_;
  std::vector<PPMXLReader::Row> entries_;
};

struct GraphicsProgram: public Graphics {
  static constexpr double timeScale = 3600.0;

  GraphicsProgram()
      : Graphics() {
    Graphics::windowWidth_ = 1024;
    Graphics::windowHeight_ = 768;
  }

  void Init() override {
    this->Graphics::Init();
    
    timeBeginning_ = chrono::steady_clock::now();
    timeIn_ = chrono::system_clock::now();

    sky_->SetTime(timeIn_);
    sky_->SetRotation(viewAngleX_, viewAngleY_);
    sky_->VertexizeStars();

    LoadStars(sky_->Stars());
  }
  
  Fallible<> Render() override {
    auto now = chrono::steady_clock::now();
    timeIn_ += std::chrono::duration_cast<std::chrono::system_clock::duration>(
        timeScale * (now - timeBeginning_)
    );
    timeBeginning_ = now;
    // timeIn_ += timeScale * (chrono::steady_clock::now() - timeBeginning_)).count();

    sky_->SetTime(timeIn_);
    sky_->SetRotation(viewAngleX_, viewAngleY_);
    sky_->VertexizeStars();

    // LoadStars(sky_->Stars());
    UpdateStars(sky_->Stars());

    RenderStars();
    if (auto rv = RenderText(); !rv) {
      return rv;
    }

    return {};
  }

  Fallible<> RenderText() {
    return textPipeline_.shader.UsingProgramme([this]() -> Fallible<> {
        glBindVertexArray(textPipeline_.vao);
        FALL_ON_GL_ERROR();

        std::stringstream ss;
        std::time_t t = std::chrono::system_clock::to_time_t(timeIn_);
        std::tm tm = *std::localtime(&t);
        ss << "fps: " << std::fixed << Fps()
           << " time: " << std::put_time(&tm, "%c %Z")
           << std::fixed << std::showpoint << std::setprecision(3)
           << " rot (x, y): "
           << '(' << viewAngleX_ / the::kRad << ", " << viewAngleY_ / the::kRad << ')';
        auto const str = ss.str();
        auto image = the::ui::RenderFont({str.c_str(), str.size()});
        if (!image) {
          ERROR() << "failed to render the text " << std::quoted(str) << ": " << image.Err();
          return image;
        }
    
        glActiveTexture(GL_TEXTURE0);
        FALL_ON_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, textPipeline_.textureID);
        FALL_ON_GL_ERROR();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width(), image->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data());
        FALL_ON_GL_ERROR();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        FALL_ON_GL_ERROR();

        return {};
      });
  }

  void HandleInput() override {
    double const rotationStep = the::kPi / 12.0 / 60.0;
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
    viewAngleX_ = std::remainder(viewAngleX_, the::kPi2);
    viewAngleY_ = std::remainder(viewAngleY_, the::kPi2);
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_Z)) {
      viewAngleX_ = -0;//the::kPi/2.0;
      viewAngleY_ =  0;//the::kPi;
    }
    if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_X)) {
      sky_->ToggleExtra();
    }
  }

  void SetSky(Starsky *sky) {
    sky_ = sky;
  }

  Fallible<> LoadShaders();

  void Deinit() override {
    this->Graphics::Deinit();

    glDeleteBuffers(1, &textPipeline_.vboVertices);
    glDeleteBuffers(1, &textPipeline_.vboIndices);
    glDeleteVertexArrays(1, &textPipeline_.vao);
    glDeleteTextures(1, &textPipeline_.textureID);
  }

 private:
  // double viewAngleX_ = -(90.0 - 53.319927) * the::kRad; // the::kPi/2.0;
  double viewAngleX_ = (90.0) * the::kRad; // the::kPi/2.0;
  double viewAngleY_ =  0; // the::kPi;

  Starsky *sky_;
  chrono::system_clock::time_point timeIn_;
  chrono::steady_clock::time_point timeBeginning_;

  struct {
    the::ui::Shader shader;
    GLuint textureID;
    GLuint vVertex;
    GLuint textureMap;
    GLuint vboVertices;
    GLuint vboIndices;
    GLuint vao;
  } textPipeline_;
};

Fallible<> GraphicsProgram::LoadShaders() {
  if (auto rv = this->Graphics::LoadShaders(); !rv)
    return rv;
    
  auto vertexShader   = the::LoadFile("lib/shaders/text.vert.glsl");
  auto fragmentShader = the::LoadFile("lib/shaders/text.frag.glsl");

  if (auto rv = textPipeline_.shader.CompileVertex(vertexShader.c_str()); !rv)
    return rv;
  if (auto rv = textPipeline_.shader.CompileFragment(fragmentShader.c_str()); !rv)
    return rv;
  if (auto rv = textPipeline_.shader.LinkProgramme(); !rv)
    return rv;

  glUseProgram(textPipeline_.shader.Programme());
  FALL_ON_GL_ERROR();

  textPipeline_.vVertex    = glGetAttribLocation(textPipeline_.shader.Programme(), "vVertex");
  FALL_ON_GL_ERROR();
  textPipeline_.textureMap = glGetUniformLocation(textPipeline_.shader.Programme(), "textureMap");
  FALL_ON_GL_ERROR();

  glUniform1i(textPipeline_.textureMap, 0);
  FALL_ON_GL_ERROR();

  GLfloat vertices[][2] = {
    {0.0, 0.0},
    {1.0, 0.0},
    {1.0, 1.0},
    {0.0, 1.0}
  };
  GLushort indices[] = {0, 1, 2, 0, 2, 3};

  glGenVertexArrays(1, &textPipeline_.vao);
  FALL_ON_GL_ERROR();
  glGenBuffers(1, &textPipeline_.vboVertices);
  FALL_ON_GL_ERROR();
  glGenBuffers(1, &textPipeline_.vboIndices);
  FALL_ON_GL_ERROR();

  glBindVertexArray(textPipeline_.vao);
  FALL_ON_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER, textPipeline_.vboVertices);
  FALL_ON_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
  FALL_ON_GL_ERROR();

  glEnableVertexAttribArray(textPipeline_.vVertex);
  FALL_ON_GL_ERROR();
  glVertexAttribPointer(textPipeline_.vVertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
  FALL_ON_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textPipeline_.vboIndices);
  FALL_ON_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
  FALL_ON_GL_ERROR();

  char const str[] = "The Universe"; //"WAVAW 春の曲";
  auto image = the::ui::RenderFont({str, sizeof(str) - 1});
  if (!image) {
    ERROR() << "failed to render the text " << std::quoted(str) << ": " << image.Err();
  }
    
  glGenTextures(1, &textPipeline_.textureID);
  FALL_ON_GL_ERROR();
  glActiveTexture(GL_TEXTURE0);
  FALL_ON_GL_ERROR();
  glBindTexture(GL_TEXTURE_2D, textPipeline_.textureID);
  FALL_ON_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  FALL_ON_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  FALL_ON_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  FALL_ON_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  FALL_ON_GL_ERROR();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width(), image->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data());
  FALL_ON_GL_ERROR();

  return {};
}

int main() {
  std::cin.sync_with_stdio(false);
  std::cin.tie(nullptr);

  GraphicsProgram graphics;

  gsl::owner<Starsky *> sky = new Starsky{};
  sky->Init();
  sky->LoadStars(std::cin);

  graphics.SetSky(sky);
  graphics.Init();
  PrintGraphicsInfo();


  if (auto rv = graphics.LoadShaders(); !rv)
    the::Panic(rv.Err().What());

  graphics.Loop();
  graphics.Deinit();

  return 0;
}
