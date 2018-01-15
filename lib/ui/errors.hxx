#pragma once

#include <sstream>
#include <tuple>

#include <GL/glew.h>

#include "lib/common.hxx"
#include "lib/errors.hxx"

#define FALL_ON_GL_ERROR(...)                                           \
  if (auto const code = glGetError(); code != GL_NO_ERROR) {            \
    using ::the::ui::OglPositionedError;                                \
    using ::the::ui::OglRuntimeError;                                   \
    if constexpr (std::tuple_size<decltype(                             \
        std::make_tuple(__VA_ARGS__))>::value > 0) {                    \
      std::stringstream ss;                                             \
      ss << OglPositionedError{                                         \
        code, __FILE__, PRETTY_FUNCTION, __LINE__                       \
      };                                                                \
      std::apply([&ss](auto ...x) {                                     \
          (..., (ss << ": " << x));                                     \
        },                                                              \
        std::make_tuple(__VA_ARGS__)                                    \
      );                                                                \
      return {OglRuntimeError{ss.str()}};                               \
    } else {                                                            \
      return {                                                          \
        OglPositionedError{code, __FILE__, PRETTY_FUNCTION, __LINE__}   \
      };                                                                \
    }                                                                   \
  }

#define PANIC_ON_GL_ERROR                                               \
  if (auto const code = glGetError(); code != GL_NO_ERROR) {            \
    using ::the::ui::OglPositionedError;                                \
    the::Panic(                                                         \
      OglPositionedError{code, __FILE__, PRETTY_FUNCTION, __LINE__}     \
    );                                                                  \
  }

namespace the::ui {

struct OglRuntimeError: public RuntimeError {
  using RuntimeError::RuntimeError;

  void What(std::ostream &os) const noexcept override {
    os << message_;
  }
};

struct OglError: public Error {
  constexpr OglError(GLuint code): code_{code} {}

  void What(std::ostream &os) const noexcept override {
    os << "glGetError returned " << OglErrorToString(code_)
       << " (" << code_ << ')';
  }

 protected:
  static
  char const * OglErrorToString(GLenum code) noexcept {
    switch (code) {
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
        return "glGetError returned unknown value";
    }
  }

  GLuint code_;
};

struct OglPositionedError: public OglError {
  constexpr OglPositionedError(GLuint code, char const *file, char const *func, int const line)
  : OglError{code}, file_{file}, func_{func}, line_{line} {}

  void What(std::ostream &os) const noexcept override {
    os << file_ << ':' << func_ << ':' << line_
       << ": " << static_cast<OglError const &>(*this);
  }

 protected:
  char const *file_;
  char const *func_;
  int  const  line_;
};

}
