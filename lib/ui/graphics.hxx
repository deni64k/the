#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <gsl.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/consts.hxx"
#include "lib/log.hxx"
#include "lib/utils.hxx"
#include "lib/mat.hxx"
#include "lib/ui/errors.hxx"
#include "lib/ui/shader.hxx"

static
char const * GlErrorToString(GLenum err) {
  switch (err) {
    case GL_NO_ERROR:
      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
    default:
      return "GlErrorToString(unknown)";
  }
}

#define PANIC_ON_GL_ERROR                                               \
  if (auto const err = glGetError(); err != GL_NO_ERROR) {              \
    std::stringstream ss;                                               \
    ss << __func__ << ':' << __LINE__ << ": glGetError=" << err         \
       << ": " << GlErrorToString(err) << '\n';                         \
    astro::Panic(ss.str());                                             \
  }

namespace astro::ui {

struct Graphics {
  struct [[gnu::packed]] Star {
    float coords[3];
    float mag;
  };

  struct StarsPipeline {
    Shader shader;
    GLuint programme;
    GLuint vbo;
    GLuint vao;
    GLuint modelToWorldMatrix;
  };

  virtual void Init() {
    using namespace std::placeholders;

    glfwSetErrorCallback(Graphics::OnGlfwErrorCallback_);

    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
      Panic(OpenGlError{"could not start GLFW3"});
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
      Panic(OpenGlError{"could not open window with GLFW3"});
    }
    glfwGetFramebufferSize(window_, &Graphics::windowWidth_, &Graphics::windowHeight_);
    glfwSetWindowSizeCallback(window_, &Graphics::OnGlfwWindowSizeCallback_);
    glfwMakeContextCurrent(window_);
    PANIC_ON_GL_ERROR;

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    PANIC_ON_GL_ERROR;
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    PANIC_ON_GL_ERROR;
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    PANIC_ON_GL_ERROR;
  }

  void LoadStars(gsl::span<Star const> const &stars) {
    size_ = stars.size();
    glGenBuffers(1, &starsPipeline_.vbo);
    PANIC_ON_GL_ERROR;
    // glBindBuffer(GL_ARRAY_BUFFER, starsPipeline_.vbo);
    // PANIC_ON_GL_ERROR;
    // glBufferData(GL_ARRAY_BUFFER,
    //              stars.size_bytes(), reinterpret_cast<GLfloat const *>(stars.data()),
    //              GL_DYNAMIC_DRAW);
    // PANIC_ON_GL_ERROR;

    glGenVertexArrays(1, &starsPipeline_.vao);
    glBindVertexArray(starsPipeline_.vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, starsPipeline_.vbo);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    PANIC_ON_GL_ERROR;
  }

  void UpdateStars(gsl::span<Star const> const &stars) {
    size_ = stars.size();
    glBindBuffer(GL_ARRAY_BUFFER, starsPipeline_.vbo);
    PANIC_ON_GL_ERROR;
    // glBufferSubData(GL_ARRAY_BUFFER, 0,
    //                 stars.size_bytes(), reinterpret_cast<GLfloat const *>(stars.data()));
    glBufferData(GL_ARRAY_BUFFER,
                 stars.size_bytes(), reinterpret_cast<GLfloat const *>(stars.data()),
                 GL_DYNAMIC_DRAW);
    PANIC_ON_GL_ERROR;
  }

  static Mat<4, 4, GLfloat> ComputeCameraMatrix() {
    float const fov = (60.0f / 2.0f) * static_cast<float>(kRad);
    float const tanFov = std::tan(fov);
    float const zNear = 0.1f;
    float const zFar = 10.0f;
    float const zRange = zNear - zFar;
    float const ratio = windowWidth_ / windowHeight_;

    // Vec3 const position = {

    // };

    return {
        1.0f / (tanFov * ratio), 0.0f,          0.0f,                     0.0f,
        0.0f,                    1.0f / tanFov, 0.0f,                     0.0f,
        0.0f,                    0.0f,          -(zNear + zFar) / zRange, 2.0f * zFar * zNear / zRange,
        0.0f,                    0.0f,          1.0f,                     0.0f
    };
  }

  void RenderStars() {
    auto mat = ComputeCameraMatrix();
    glProgramUniformMatrix4fv(starsPipeline_.programme, starsPipeline_.modelToWorldMatrix, 1, GL_FALSE, &mat[0][0]);
    PANIC_ON_GL_ERROR;

    glUseProgram(starsPipeline_.programme);
    PANIC_ON_GL_ERROR;

    glBindVertexArray(starsPipeline_.vao);
    PANIC_ON_GL_ERROR;

    // GLfloat attribColor[] = {
    //   std::sin(0.0000 + double{now} * 0.0001),
    //   std::cos(0.0005 + double{now} * 0.0002),
    //   std::cos(0.0010 + double{now} * 0.0003),
    //   1.0
    // };
    // glVertexAttrib4fv(1, attribColor);

    // Draw points 0-3 from the currently bound VAO with current in-use shader.
    // glPointSize(2.5f);
    glDrawArrays(GL_POINTS, 0, GLuint(size_));
    PANIC_ON_GL_ERROR;
  }

  Fallible<> LoadShaders();

  virtual void Deinit() {
    // close GL context and any other GLFW resources
    if (!glfwInitialized_) {
      glfwTerminate();
      glfwInitialized_ = false;
    }
    // TODO: Delete only if it has been compiled before.
    glDeleteProgram(starsPipeline_.programme);
    glDeleteVertexArrays(1, &starsPipeline_.vao);
  }

  virtual Fallible<> Render() = 0;
  virtual void HandleInput() = 0;

  virtual void Loop() final {
    static constexpr int perfectFps = 60;

    while (!glfwWindowShouldClose(window_)) {
      auto const renderingStartedAt = std::chrono::steady_clock::now();

      for (int iFrame = 0; iFrame < perfectFps; ++iFrame) {
        HandleInput();

        glViewport(0, 0, windowWidth_, windowHeight_);

        // Wipe the drawing surface clear.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (auto rv = Render(); !rv) {
          ERROR() << "failed to render a frame: " << rv.Err() << '\n';
        }

        // Update other events like input handling.
        glfwPollEvents();

        // Put the stuff we've been drawing onto the display.
        glfwSwapBuffers(window_);
      }

      auto renderingTook = std::chrono::steady_clock::now() - renderingStartedAt;
      fps_ = static_cast<double>(perfectFps) / std::chrono::duration<double>(renderingTook).count();
    }
  }

  static
  int WindowWidth() {
    return windowWidth_;
  }
  static
  int WindowHeight() {
    return windowHeight_;
  }

  double Fps() { return fps_; }

 protected:
  static
  void OnGlfwWindowSizeCallback_(GLFWwindow *window, int width, int height) {
    glfwGetFramebufferSize(window, &width, &height);
    // auto const dim = std::min(width, height);
    windowWidth_ = width;
    windowHeight_ = height;
    // TODO: update any perspective matrices used here
  }

  static
  void OnGlfwErrorCallback_(int error, char const *description) {
    ERROR() << "GLFW: code=" << error << ": " << description << '\n';
  }

  GLFWwindow *window_;
  std::size_t size_;

  StarsPipeline starsPipeline_;

  static int windowWidth_;
  static int windowHeight_;

 private:
  double fps_ = 0.0;
  bool glfwInitialized_ = false;
};

}
