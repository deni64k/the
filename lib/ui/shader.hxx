#pragma once

#include <functional>

#include <GL/glew.h>

#include "lib/utils.hxx"

namespace the::ui {

struct Shader final {
  Shader();
  ~Shader();

  Fallible<> CompileVertex(gsl::span<char const> shaderSource);
  Fallible<> CompileFragment(gsl::span<char const> shaderSource);
  Fallible<> LinkProgramme();
  Fallible<> UsingProgramme(std::function<Fallible<> ()> const fn) const;

  inline GLuint Programme() const { return programme_; }

 private:
  GLuint vertexShader_   = 0;
  GLuint fragmentShader_ = 0;
  GLuint programme_      = 0;
  GLuint vbo_;
  GLuint vao_;
};

}
