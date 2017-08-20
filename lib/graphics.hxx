#pragma once

#include "log.hxx"
#include "utils.hxx"

#include <functional>
#include <gsl.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace astro {

struct Graphics {
  struct Star {
    float coords[3];
    float mag;
  } __attribute__((packed));

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

  void LoadStars(gsl::span<Star const> const &stars) {
    size_ = stars.size();
    glGenBuffers(1, starsVbo_);
    glBindBuffer(GL_ARRAY_BUFFER, starsVbo_[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 stars.size_bytes(), reinterpret_cast<GLfloat const *>(stars.data()),
                 GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, starsVao_);
    glBindVertexArray(starsVao_[0]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, starsVbo_[0]);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  void UpdateStars(gsl::span<Star const> const &stars) {
    size_ = stars.size();
    glBindBuffer(GL_ARRAY_BUFFER, starsVbo_[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    stars.size_bytes(), reinterpret_cast<GLfloat const *>(stars.data()));
  }

  void RenderStars() {
    glUseProgram(starsProgramme_);
    glBindVertexArray(starsVao_[0]);

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
  }
  
  void LoadShaders() {    
    std::string vertex_shader{LoadFile("lib/shaders/vertex.glsl")};
    std::string fragment_shader{LoadFile("lib/shaders/fragment.glsl")};

    starsProgramme_ = CompileShaders(vertex_shader.c_str(), fragment_shader.c_str());
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
    glDeleteProgram(starsProgramme_);
    glDeleteVertexArrays(1, starsVao_);
    glDeleteVertexArrays(1, starsVbo_);
  }

  virtual void Render() = 0;
  virtual void HandleInput() = 0;
  
  void Loop() {
    while (!glfwWindowShouldClose(window_)) {
      glViewport(0, 0, windowWidth_, windowHeight_);

      // Wipe the drawing surface clear.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Render();

      // Update other events like input handling.
      glfwPollEvents();

      // Put the stuff we've been drawing onto the display.
      glfwSwapBuffers(window_);

      HandleInput();
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
  
 protected:
  static
  void OnGlfwWindowSizeCallback_(GLFWwindow *window, int width, int height) {
    glfwGetFramebufferSize(window, &width, &height);
    auto const dim = std::min(width, height);
    windowWidth_ = dim;
    windowHeight_ = dim;
    // TODO: update any perspective matrices used here
  }
  
  static
  void OnGlfwErrorCallback_(int error, char const *description) {
    ERROR("GLFW: code %i msg: %s\n", error, description);
  }
  
  GLFWwindow *window_;
  std::size_t size_;
  GLuint starsProgramme_;
  GLuint starsVbo_[1];
  GLuint starsVao_[1];

  static int windowWidth_;
  static int windowHeight_;

 private:
  bool glfwInitialized_ = false;
};

}
