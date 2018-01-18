#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <gsl.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/consts.hxx"
#include "lib/errors.hxx"
#include "lib/logging.hxx"
#include "lib/mat.hxx"
#include "lib/ui/errors.hxx"
#include "lib/ui/shader.hxx"

namespace the::ui {

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

  virtual OglFallible<> Init();
  virtual OglFallible<> Deinit();

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
    static float const fov = (60.0f / 2.0f) * static_cast<float>(kRad);
    static float const tanFov = std::tan(fov);
    static float const zNear = 0.1f;
    static float const zFar = 10.0f;
    static float const zRange = zNear - zFar;
    float const ratio = static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_);

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

  OglFallible<> LoadShaders();

  virtual OglFallible<> Render() = 0;
  virtual OglFallible<> HandleInput() = 0;

  OglFallible<> Loop();

  static int WindowWidth()  { return windowWidth_; }
  static int WindowHeight() { return windowHeight_; }
  static float Dpu() {
    return 1.0f / (static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_));
  }

  inline double Fps() const { return 1.0 / std::chrono::duration<double>(lastFrameTook_).count(); }


 protected:
  static void OnGlfwWindowSizeCallback_(GLFWwindow *window, int width, int height);
  static void OnGlfwErrorCallback_(int error, char const *description);

  GLFWwindow *window_;
  std::size_t size_;

  StarsPipeline starsPipeline_;

  static int windowWidth_;
  static int windowHeight_;

 private:
  std::chrono::steady_clock::duration lastFrameTook_ = std::chrono::seconds{1};
  bool glfwInitialized_ = false;
};

}
