#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/consts.hxx"
#include "lib/sun.hxx"
#include "lib/time.hxx"
#include "lib/spheric.hxx"

// Rotterdam's home.
static double const kLat = 51.917337 * astro::kRad;
static double const kLng = 4.474634  * astro::kRad;
static std::size_t const kSkySize = 1000;

// PPMXL catalogue
// 910468688 records

struct Starsky {
  struct Row {
    Row(std::size_t const skySize, int* const ptr)
        : skySize_(skySize)
        , ptr_(ptr)
    {}

    int& operator [] (std::size_t const i) {
      assert(i < skySize_ && "Index mustn't exceed the sky size.");
      return *(ptr_ + i);
    }

    int operator [] (std::size_t const i) const {
      assert(i < skySize_ && "Index mustn't exceed the sky size.");
      return *(ptr_ + i);
    }
    
   private:
    std::size_t const skySize_;
    int* const ptr_;
  };
  
  Starsky()
      : skySize_(kSkySize)
      , skyRaw_(new int[skySize_*skySize_])
  {
    Reset();
  }

  std::size_t Size() const {
    return skySize_;
  }
  
  void Reset() {
    ::bzero(skyRaw_, skySize_ * skySize_ * sizeof(*skyRaw_));
  }

  Row operator [] (std::size_t const i) const {
    assert(i < skySize_ && "Index mustn't exceed the sky size.");
    return Row(skySize_, skyRaw_ + skySize_ * i);
  }
  
  void MarkStar(double ra, double decl, double gmst, int radius = 1, int mag = 15) {
    radius |= 1;  // Restrict radius to odd numbers.
    
    // ra *= astro::kRad;
    double const tau = gmst - ra;
    double az, elev;
    astro::Equ2Hor(decl, tau, kLat, elev, az);

    // elev += astro::kPi/4.0;
    // elev = -elev;
    // elev = std::remainder(elev, astro::kPi2);

    // elev = std::remainder(elev - astro::kPi/4.0, astro::kPi2);
    az = std::remainder(az + astro::kPi*3.0/2.0 - astro::kPi/8.0, astro::kPi2);

    if (!(az < 0.0 || az > astro::kPi))
      return;
    // if (elev < -astro::kPi/2.0 || elev > astro::kPi/2.0)
    //   return;

    int x = std::cos(elev) * std::cos(az) * skySize_/2 + skySize_/2;
    int y = std::sin(elev) * skySize_/2 + skySize_/2;
    if (x >= 0 && x < skySize_ &&
        y >= 0 && y < skySize_) {
      y = skySize_ - y;
      for (int i = -radius; i < radius; ++i) {
        if (x+i >= 0 && x+i < skySize_)
          (*this)[x+i][y] = mag;
        if (y+i >= 0 && y+i < skySize_)
          (*this)[x][y+i] = mag;
      }
    }
  }

 private:
  std::size_t const skySize_;
  int* skyRaw_;
};

struct PPMXLReader {
  PPMXLReader(std::istream& is, std::size_t limit = 0):
      is_(is),
      limit_(limit)
  {}

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
    std::istream& operator >> (std::istream& is, Row& row) {
      char delim;
      std::string::size_type pos = 0;
      
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
      char const* delim = "|";
      char* tok = std::strtok(s.data(), delim);

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
  PPMXLReader& operator >> (PPMXLReader& is, Row& row) {
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
  std::istream& is_;
  std::size_t limit_ = 0;
  std::size_t pos_ = 0;
};

bool gl_log(const char* message, ...) {
  va_list argptr;
  va_start(argptr, message);
  vfprintf(stderr, message, argptr);
  va_end(argptr);
  return true;
}

bool gl_log_err(const char* message, ...) {
  va_list argptr;
  va_start(argptr, message);
  vfprintf(stderr, message, argptr);
  va_end(argptr);
  return true;
}

std::string LoadFile(char const *fpath) {
  std::string s;
  if (std::ifstream is{fpath, std::ios::binary | std::ios::ate}) {
    fprintf(stderr, "reading %s\n", fpath);

    auto size = is.tellg();
    s.resize(size);
    is.seekg(0);
    if (!is.read(s.data(), size))
      throw std::runtime_error("could not read file");
  }
  return std::move(s);
}

struct Graphics {
  void Init() {
    using namespace std::placeholders;

    glfwSetErrorCallback(OnGlfwErrorCallback);

    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
      throw std::runtime_error("could not start GLFW3");
    }
    glfwInitialized_ = true;

#ifdef __APPLE__
    // Get the newest available version of OpenGL on Apple,
    // which will be 4.1 or 3.3 on Mavericks, and 3.2 on
    // pre-Mavericks systems.
    // On other systems it will tend to pick 3.2 instead of
    // the newest version, which is unhelpful.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    glfwWindowHint(GLFW_SAMPLES, 4);

    // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    // GLFWvidmode const *vmode = glfwGetVideoMode(monitor);
    // window_ = glfwCreateWindow(
    //     vmode->width, vmode->height, "Stars sky", monitor, NULL);
    window_ = glfwCreateWindow(
        windowWidth_, windowHeight_, "Stars sky", nullptr, nullptr);
    if (!window_) {
      throw std::runtime_error("could not open window with GLFW3");
    }
    glfwSetWindowSizeCallback(window_, &Graphics::OnGlfwWindowSizeCallback);
    glfwMakeContextCurrent(window_);
                                  
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
  }

  void LoadBuffers() {
    float points[] = {
       0.0f,  0.5f,  0.0f,
       0.5f, -0.5f,  0.0f,
       0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f,
       0.0f,  0.5f,  0.0f
    };
      
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  
  void LoadShaders() {    
    std::string vertex_shader(LoadFile("lib/shaders/vertex.glsl"));
    std::string fragment_shader(LoadFile("lib/shaders/fragment.glsl"));

    shaderProgramme_ = CompileShaders(vertex_shader.c_str(), fragment_shader.c_str());
  }

  GLuint CompileShaders(char const *vertex_shader, char const *fragment_shader) {
    // Create and compile vertex shader.
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    // Create and compile fragment shader.
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    // Create program, attach shaders to it, and link it.
    GLuint program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glLinkProgram(program);

    // Delete the shaders as the program has them now.
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
  }

  void Run() {
    std::clock_t now;
    while (!glfwWindowShouldClose(window_)) {
      now = std::clock();
      
      // Wipe the drawing surface clear.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glViewport(0, 0, windowWidth_, windowHeight_);
      glUseProgram(shaderProgramme_);
      glBindVertexArray(vao_);

      GLfloat attribColor[] = {
        std::sin(0.0000f + float{now} * 0.0001f),
        std::cos(0.0005f + float{now} * 0.0002f),
        std::cos(0.0010f + float{now} * 0.0003f),
        1.0f
      };
      glVertexAttrib4fv(1, attribColor);

      // Draw points 0-3 from the currently bound VAO with current in-use shader.
      glPointSize(25.0f);
      glDrawArrays(GL_POINTS, 0, 6);

      // Update other events like input handling.
      glfwPollEvents();

      // Put the stuff we've been drawing onto the display.
      glfwSwapBuffers(window_);

      if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window_, 1);
      }
    }
  }

  void Deinit() {
    // close GL context and any other GLFW resources
    if (!glfwInitialized_) {
      glfwTerminate();
      glfwInitialized_ = false;
    }
    // TODO: Delete only if it has been compiled before.
    glDeleteProgram(shaderProgramme_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteVertexArrays(1, &vbo_);
  }
  
 private:
  static
  void OnGlfwWindowSizeCallback(GLFWwindow *window, int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    // TODO: update any perspective matrices used here
  }
  
  static
  void OnGlfwErrorCallback(int error, char const *description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
  }
  
  GLFWwindow *window_;
  GLuint vbo_ = 0;
  GLuint vao_ = 0;
  GLuint shaderProgramme_;
  bool glfwInitialized_ = false;

  static int windowWidth_;
  static int windowHeight_;
};

int Graphics::windowWidth_ = 640;
int Graphics::windowHeight_ = 480;

void PrintGraphicsInfo() {
  GLubyte const *renderer = glGetString(GL_RENDERER); // get renderer string
  GLubyte const *version = glGetString(GL_VERSION); // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  GLfloat sizes[2];  // stores supported point size range
  GLfloat step;      // stores supported point size increments
  glGetFloatv(GL_POINT_SIZE_RANGE,sizes);
  glGetFloatv(GL_POINT_SIZE_GRANULARITY,&step);
  printf("Supported point size range: %f ... %f\n", sizes[0], sizes[1]);
  printf("Supported point size increments: %f\n", step);
  
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
  gl_log("GL Context Params:\n");
  char msg[256];
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    gl_log("%s %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  gl_log("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  gl_log("%s %u\n", names[11], (unsigned int)s);
  gl_log("-----------------------------\n");
}

int main() {
  // std::cin.sync_with_stdio(false);
  // std::cin.tie(nullptr);

  Graphics graphics;

  try {
    graphics.Init();
    PrintGraphicsInfo();
    graphics.LoadBuffers();
    graphics.LoadShaders();
    graphics.Run();
    graphics.Deinit();
  } catch (std::exception *ex) {
    fprintf(stderr, "ERROR: %s\n", ex);
    graphics.Deinit();
    return 1;
  }

  return 0;

  // ----
  auto sky = std::make_unique<Starsky>();

  std::time_t now = std::time(nullptr);

  std::tm tm = *std::gmtime(&now);
  double const mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                tm.tm_hour, tm.tm_min, tm.tm_sec);
  double const gmst = astro::GMST(mjd) + kLng;

  std::cerr << "Latitude: " << astro::FormatDMS(kLat * astro::kDeg) << '\n';
  std::cerr << "Altitude: " << astro::FormatDMS(kLng * astro::kDeg) << '\n';
  std::cerr << "Time: " << std::put_time(&tm, "%c %Z") << '\n';
  std::cerr << "MJD: " << mjd << '\n';
  std::cerr << "GMST: " << astro::FormatHMS(gmst * astro::kDeg) << '\n';
  std::cerr << '\n';

  auto markMap = ([&](double ra, double decl, int radius = 1, int mag = 15) {
    sky->MarkStar(ra, decl, gmst, radius, mag);
  });
  auto markMapDeg = ([&](double ra, double decl, int radius = 1) {
    markMap(ra * astro::kRad, decl * astro::kRad, radius);
  });

  std::size_t nStars{0}, nTotal{0};
  PPMXLReader reader(std::cin);
  PPMXLReader::Row data;
  while (reader >> data) {
    ++nTotal;
    if (nTotal % 100000 == 0) {
      std::cerr << '\r' << nStars << '/' << nTotal << ' ' << float{100.0f * nStars / nTotal} << '%';
      std::cerr.flush();
    }

    // Filter out stars invisible for naked eye.
    // if (data.Jmag <= 0.0 || data.Jmag >= 2.5)
    //   continue;

    double ra, delta;
    ra    = data.RaJ2000 * astro::kRad;
    delta = data.DecJ2000 * astro::kRad;
    
    for (int offset = 0; offset < 1; ++offset) {
      auto const nw = now + offset * 60;
      std::tm tm = *std::gmtime(&nw);
      double const mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                    tm.tm_hour, tm.tm_min, tm.tm_sec);
      double const gmst = astro::GMST(mjd) + kLng;
      sky->MarkStar(ra, delta, gmst, 1, data.Jmag / (2.5 / 10.0) + 5.0);
    }

    ++nStars;
    continue;

      // double const tau = gmst - ra;

      // // std::cerr << "Star #" << data.Ipix << '\n';
      // // std::cerr << "RA:       " << astro::FormatHMS(ra * astro::kDeg) << '\t';
      // // std::cerr << "Delta:    " << astro::FormatDMS(delta * astro::kDeg) << '\n';

      // double az, elev;
      // astro::Equ2Hor(delta, tau, kLat, elev, az);

      // // std::cerr << "Azimuth:  " << astro::FormatDMS(az * astro::kDeg) << '\t';
      // // std::cerr << "Altitude: " << astro::FormatDMS(elev * astro::kDeg) << '\n';
      // // std::cerr << '\n';

      // // int x = std::cos(elev) * std::cos(az) * skySize/2 + skySize/2;
      // // int y = std::sin(elev) * skySize/2 + skySize/2;
      // // if (x >= 0 && x < skySize &&
      // //     y >= 0 && y < skySize) {
      // //   y = skySize - y;
      // //   sky[x][y] = data.Jmag / (2.5 / 10.0) + 5.0;
      // // }
  }
  std::cerr << std::endl;
  
  // Deneb
  // markMapDeg(310.357978889, 45.2803369444, 5);

  // Cassiopeiea
  markMapDeg(astro::FromDMS(0, 40, 30.4405)  * 15.0, astro::FromDMS(56, 32, 14.3920), 3);
  markMapDeg(astro::FromDMS(0,  9, 10.68518) * 15.0, astro::FromDMS(59,  8, 59.2120), 3);
  markMapDeg(astro::FromDMS(0, 56, 42.50108) * 15.0, astro::FromDMS(60, 43,  0.2984), 3);
  markMapDeg(astro::FromDMS(1, 25, 48.95147) * 15.0, astro::FromDMS(60, 14,  7.0225), 3);
  markMapDeg(astro::FromDMS(1, 54, 23.72567) * 15.0, astro::FromDMS(63, 40, 12.3628), 3);

  // Orion's Belt
  // markMapDeg(astro::FromDMS(5, 32,  0.40009) * 15.0, astro::FromDMS( 0, -17, 56.7424));
  // markMapDeg(astro::FromDMS(5, 36, 12.8)     * 15.0, astro::FromDMS(-1,  12,  6.9), 5);
  // markMapDeg(astro::FromDMS(5, 40, 45.52666) * 15.0, astro::FromDMS(-1,  56, 34.2649));

  for (int i = 0; i < 24; ++i) {
    markMapDeg(astro::FromDMS(i, i, i) * 15.0, astro::FromDMS( 90, 0, 0), 10);
    markMapDeg(astro::FromDMS(i, i, i) * 15.0, astro::FromDMS(-90, 0, 0), 10);
  }

  // Sun
  {
    for (int offset = 0; offset < 100; ++offset) {
      auto const nw = now + offset * 3600;
      std::tm tm = *std::gmtime(&nw);
      double mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                              tm.tm_hour, tm.tm_min, tm.tm_sec);
      double epoch = (mjd - astro::kMJD_J2000) / 36525.0;
      auto sun = astro::MakePolar(astro::Ecl2EquMatrix(epoch) * astro::SunPos(epoch));
      double h, az;
      astro::Equ2Hor(sun.Theta, sun.Phi, kLat, h, az);
      markMap(az, h, 25);
    }
  }

  // Output PPM image of the sky.
  std::cout << "P2\n";
  std::cout << sky->Size() << ' ' << sky->Size() << '\n';
  std::cout << "15" << '\n';
  for (int i = 0; i < sky->Size(); ++i) {
    for (int j = 0; j < sky->Size(); ++j) {
      std::cout << (*sky)[i][j] << ' ';
    }
    std::cout << '\n';
  }

  std::cerr << "Nr of stars on map: " << nStars << std::endl;

  return 0;
}
