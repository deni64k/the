#include <gsl.h>
#include <vector>

#include "lib/ui/errors.hxx"
#include "lib/ui/shader.hxx"
#include "lib/utils.hxx"
#include "lib/logging.hxx"

namespace the::ui {

namespace {
OglFallible<GLuint> CompileShader(GLenum shaderType, gsl::span<char const> shaderSource) {
  GLchar const *strings[] = {shaderSource.data()};
  GLint  const  lengths[] = {static_cast<GLint>(shaderSource.size())};

  GLuint shader = glCreateShader(shaderType);
  FALL_ON_GL_ERROR();
  glShaderSource(shader, 1, strings, lengths);
  FALL_ON_GL_ERROR();
  glCompileShader(shader);
  FALL_ON_GL_ERROR();

  GLint isCompiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  FALL_ON_GL_ERROR();
  if (isCompiled != GL_TRUE) {
    GLsizei logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    FALL_ON_GL_ERROR();

    std::vector<GLchar> log(logLength);
    glGetShaderInfoLog(shader, logLength, &logLength, log.data());
    FALL_ON_GL_ERROR();

    return {RuntimeError{std::string{log.data(), static_cast<std::size_t>(logLength)}}};
  }
  
  return {shader};
}

}

Shader::Shader() {
}

Shader::~Shader() {
  glDeleteShader(vertexShader_);
  glDeleteShader(fragmentShader_);
}

OglFallible<> Shader::CompileVertex(gsl::span<char const> shaderSource) {
  if (!vertexShader_) {
    glDeleteShader(vertexShader_);
    vertexShader_ = 0;
  }

  if (auto rv = CompileShader(GL_VERTEX_SHADER, shaderSource); !rv) {
    return std::move(rv);
  } else {
    vertexShader_ = *rv;
  }

  return {};
}

OglFallible<> Shader::CompileFragment(gsl::span<char const> shaderSource) {
  if (!fragmentShader_) {
    glDeleteShader(fragmentShader_);
    fragmentShader_ = 0;
  }

  if (auto rv = CompileShader(GL_FRAGMENT_SHADER, shaderSource); !rv) {
    return std::move(rv);
  } else {
    fragmentShader_ = *rv;
  }

  return {};
}

OglFallible<> Shader::LinkProgramme() {
  GLuint programme = glCreateProgram();
  FALL_ON_GL_ERROR();
  glAttachShader(programme, fragmentShader_);
  FALL_ON_GL_ERROR();
  glAttachShader(programme, vertexShader_);
  FALL_ON_GL_ERROR();
  glLinkProgram(programme);
  FALL_ON_GL_ERROR();

  GLint isLinked;
  glGetProgramiv(programme, GL_LINK_STATUS, &isLinked);
  FALL_ON_GL_ERROR();
  if (isLinked == GL_FALSE) {
    GLint logLength;
    glGetProgramiv(programme, GL_INFO_LOG_LENGTH, &logLength);

    // The logSize includes the NULL character.
    std::vector<GLchar> log(logLength);
    glGetProgramInfoLog(programme, logLength, &logLength, log.data());

    return {RuntimeError{std::string{log.data(), static_cast<std::size_t>(logLength)}}};
  }

  programme_ = programme;
  
  return {};
}

OglFallible<> Shader::UsingProgramme(std::function<OglFallible<> ()> const fn) const {
  if (!programme_) {
    return {RuntimeError{"use of shader programme while it is not linked"}};
  }

  glUseProgram(programme_);

  if (auto rv = fn(); !rv) {
    return std::move(rv);
  }

  glUseProgram(0);

  return {};
}

}
