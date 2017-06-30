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
#include "lib/ppmxlreader.hxx"

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

void Log(char const *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vfprintf(stderr, format, argptr);
  va_end(argptr);
}

#define DEBUG(format, args...) Log("DEBUG: %s:%d: " format, __func__, __LINE__, ##args)
#define ERROR(format, args...) Log("DEBUG: " format, ##args)
#define INFO(format, args...)  Log("INFO: " format, ##args)

struct Starsky;

struct Vertex {
  float coords[3];
  float mag;
} __attribute__((packed));
// typedef GLfloat Vertex[4];

struct Graphics {
  void Init() {
    using namespace std::placeholders;

    glfwSetErrorCallback(Graphics::OnGlfwErrorCallback_);

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
        WindowWidth(), WindowHeight(), "Stars sky", nullptr, nullptr);
    if (!window_) {
      throw std::runtime_error("could not open window with GLFW3");
    }
    glfwGetFramebufferSize(window_, &Graphics::windowWidth_, &Graphics::windowHeight_);
    glfwSetWindowSizeCallback(window_, &Graphics::OnGlfwWindowSizeCallback_);
    glfwMakeContextCurrent(window_);
                                  
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
  }

  void LoadPoints(GLfloat *points, GLuint size) {
    size_ = size;
    glGenBuffers(1, vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * size, points, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, vao_);
    glBindVertexArray(vao_[0]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  void UpdatePoints(GLfloat *points, GLuint size) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * size, points);
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

  void Deinit() {
    // close GL context and any other GLFW resources
    if (!glfwInitialized_) {
      glfwTerminate();
      glfwInitialized_ = false;
    }
    // TODO: Delete only if it has been compiled before.
    glDeleteProgram(shaderProgramme_);
    glDeleteVertexArrays(1, vao_);
    glDeleteVertexArrays(1, vbo_);
  }

  static
  int WindowWidth() {
    return windowWidth_;
  }
  static
  int WindowHeight() {
    return windowHeight_;
  }
  
 protected:
  static
  void OnGlfwWindowSizeCallback_(GLFWwindow *window, int width, int height) {
    DEBUG("%s: window resized: width=%i height=%i\n", __FUNCTION__, width, height);
    glfwGetFramebufferSize(window, &width, &height);
    auto const dim = std::min(width, height);
    DEBUG("%s: glfwGetFramebufferSize called: width=%i height=%i\n", __FUNCTION__, width, height);
    windowWidth_ = dim;
    windowHeight_ = dim;
    // TODO: update any perspective matrices used here
  }
  
  static
  void OnGlfwErrorCallback_(int error, char const *description) {
    ERROR("GLFW: code %i msg: %s\n", error, description);
  }
  
  GLFWwindow *window_;
  GLuint vbo_[1];
  GLuint vao_[1];
  GLuint shaderProgramme_;
  bool glfwInitialized_ = false;
  GLuint size_;

  static int windowWidth_;
  static int windowHeight_;
};

int Graphics::windowWidth_ = 540;
int Graphics::windowHeight_ = 540;

void PrintGraphicsInfo() {
  GLubyte const *renderer = glGetString(GL_RENDERER); // get renderer string
  GLubyte const *version = glGetString(GL_VERSION); // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  GLfloat sizes[2];  // stores supported point size range
  GLfloat step;      // stores supported point size increments
  glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
  glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
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
  INFO("GL Context Params:\n");
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    INFO("%s %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  INFO("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  INFO("%s %u\n", names[11], (unsigned int)s);
  INFO("-----------------------------\n");
}

using astro::PPMXLReader;

struct Starsky {
  Starsky()
  {}

  void Init() {
    time_ = std::time(nullptr);

    std::tm tm = *std::gmtime(&time_);
    double mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                            tm.tm_hour, tm.tm_min, tm.tm_sec);
    double gmst = astro::GMST(mjd) + positionLongitude_;

    std::cerr << "Latitude: " << astro::FormatDMS(positionLatitude_ * astro::kDeg) << '\n';
    std::cerr << "Altitude: " << astro::FormatDMS(positionLongitude_ * astro::kDeg) << '\n';
    std::cerr << "Time: " << std::put_time(&tm, "%c %Z") << '\n';
    std::cerr << "MJD: " << mjd << '\n';
    std::cerr << "GMST: " << astro::FormatHMS(gmst * astro::kDeg) << '\n';
    std::cerr << '\n';
  }
  
  void LoadStars(std::istream &is) {
    std::size_t nStars{0};
    PPMXLReader reader(is);
    PPMXLReader::Row data;
    while (reader >> data) {
      entries_.push_back(data);
    }
    INFO("%d stars loaded from the catalogue\n", entries_.size());
  }

  void SetTime(std::time_t time) {
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

    std::tm tm = *std::gmtime(&time_);
    double const mjd = astro::MJD(tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
                                  tm.tm_hour, tm.tm_min, tm.tm_sec);
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
    
      ProcessStar(ra, delta, gmst, data.Jmag / (2.5 / 10.0) + 5.0);
      
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
    ProcessStar(0.0,  astro::kPi/2.0, gmst, 40);
    // ProcessStar(0.0, -astro::kPi/2.0, gmst, 40);

    // Deneb
    // ProcessStar(310.357978889, 45.2803369444, 5);

    // Orion's Belt
    // ProcessStar(astro::FromDMS(5, 32,  0.40009) * 15.0, astro::FromDMS( 0, -17, 56.7424));
    // ProcessStar(astro::FromDMS(5, 36, 12.8)     * 15.0, astro::FromDMS(-1,  12,  6.9), 5);
    // ProcessStar(astro::FromDMS(5, 40, 45.52666) * 15.0, astro::FromDMS(-1,  56, 34.2649));

    // Cassiopeiea
    ProcessStar(astro::FromDMS(0, 40, 30.4405)  * 15.0 * astro::kRad, astro::FromDMS(56, 32, 14.3920) * astro::kRad, gmst, 30);
    ProcessStar(astro::FromDMS(0,  9, 10.68518) * 15.0 * astro::kRad, astro::FromDMS(59,  8, 59.2120) * astro::kRad, gmst, 30);
    ProcessStar(astro::FromDMS(0, 56, 42.50108) * 15.0 * astro::kRad, astro::FromDMS(60, 43,  0.2984) * astro::kRad, gmst, 30);
    ProcessStar(astro::FromDMS(1, 25, 48.95147) * 15.0 * astro::kRad, astro::FromDMS(60, 14,  7.0225) * astro::kRad, gmst, 30);
    ProcessStar(astro::FromDMS(1, 54, 23.72567) * 15.0 * astro::kRad, astro::FromDMS(63, 40, 12.3628) * astro::kRad, gmst, 30);

    // Sun
    {
      double epoch = (mjd - astro::kMJD_J2000) / 36525.0;
      auto vec = astro::SunPos(epoch);
      vec = astro::Ecl2EquMatrix(epoch) * vec;
      // The z axis is directed towards to the north celestial polar.
      // To align with the model axis it has to be rotated relatively x on 90 degrees.
      vec = astro::Mat3::RotateX(-astro::kPi/2.0) * vec;
      // The y axis has got reversed direction.
      vec[1] = -vec[1];
      auto sun = astro::MakePolar(vec);
      double h, az;
      astro::Equ2Hor(sun.Theta, sun.Phi, positionLatitude_, h, az);
      ProcessStar(az, h, gmst, 75);
    }
  }
  
  void ProcessStar(double ra, double decl, double gmst, int mag) {
    double const tau = gmst - ra;
    double az, elev;
    astro::Equ2Hor(decl, tau, positionLatitude_, elev, az);

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

    if (y < 0)
      return;

    // float d = x*x + y*y + z*z;
    
    // DEBUG("vertexing a star: x=% .08f, y=% .08f, z=% .08f, mag=%2i, d=%f\n", x, y, z, mag, d);

    Vertex v{x, y, z, float{mag}/3.15f};
    stars_.push_back(v);
  }

  void Reset() {
    stars_.clear();
  }

  GLfloat * Data() {
    return reinterpret_cast<GLfloat *>(stars_.data());
  }
  GLuint Size() const {
    return GLuint{stars_.size()};
  }

 protected:
  // Dublin's home.
  double const positionLatitude_  = 53.319927 * astro::kRad;
  double const positionLongitude_ = -6.264353 * astro::kRad;

  std::time_t time_ = 0;
  bool showExtra_ = true;

  // Look at the North polar star.
  double viewAngleX_ = 0.0;
  double viewAngleY_ = 0.0;

  std::vector<Vertex> stars_;
  std::vector<PPMXLReader::Row> entries_;
};

struct GraphicsProgram : public Graphics {
  void Run(Starsky &sky) {    
    std::time_t now;
    while (!glfwWindowShouldClose(window_)) {
      now = std::time(nullptr);

      glViewport(0, 0, WindowWidth(), WindowHeight());

      // Wipe the drawing surface clear.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      sky.SetTime(now);
      sky.SetRotation(viewAngleX_, viewAngleY_);
      sky.VertexizeStars();
      LoadPoints(sky.Data(), sky.Size());

      glUseProgram(shaderProgramme_);
      glBindVertexArray(vao_[0]);

      // GLfloat attribColor[] = {
      //   std::sin(0.0000 + double{now} * 0.0001),
      //   std::cos(0.0005 + double{now} * 0.0002),
      //   std::cos(0.0010 + double{now} * 0.0003),
      //   1.0
      // };
      // glVertexAttrib4fv(1, attribColor);

      // Draw points 0-3 from the currently bound VAO with current in-use shader.
      // glPointSize(2.5f);
      glDrawArrays(GL_POINTS, 0, size_);

      // Update other events like input handling.
      glfwPollEvents();

      // Put the stuff we've been drawing onto the display.
      glfwSwapBuffers(window_);

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
        sky.ToggleExtra();
      }
    }
  }

 private:
  double viewAngleX_ = -0;//astro::kPi/2.0;
  double viewAngleY_ =  0;// astro::kPi;
};

int main() {
  // std::cin.sync_with_stdio(false);
  // std::cin.tie(nullptr);

  GraphicsProgram graphics;

  try {
    graphics.Init();
    PrintGraphicsInfo();

    auto sky = std::make_unique<Starsky>();
    sky->Init();
    sky->LoadStars(std::cin);

    graphics.LoadShaders();
    graphics.Run(*sky);
    graphics.Deinit();
  } catch (std::exception const &ex) {
    fprintf(stderr, "ERROR: %s\n", ex.what());
    graphics.Deinit();
    return 1;
  }

  return 0;
}
