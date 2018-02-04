#pragma once

#include <functional>

#include <GL/glew.h>

#include "the/lib/common/utils.hxx"

namespace the::ui {

struct Shader final {
  Shader();
  ~Shader();

  OglFallible<> CompileVertex(gsl::span<char const> shaderSource);
  OglFallible<> CompileFragment(gsl::span<char const> shaderSource);
  OglFallible<> LinkProgramme();
  OglFallible<> UsingProgramme(std::function<OglFallible<> ()> const fn) const;

  inline GLuint Programme() const { return programme_; }

 private:
  GLuint vertexShader_   = 0;
  GLuint fragmentShader_ = 0;
  GLuint programme_      = 0;
  GLuint vbo_;
  GLuint vao_;
};

}
