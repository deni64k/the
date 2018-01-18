#pragma once

#include <sstream>
#include <tuple>

#include <GL/glew.h>

#include "lib/common.hxx"
#include "lib/errors.hxx"

#define FALL_ON_GL_ERROR(...)                                           \
  if (auto const code = glGetError(); code != GL_NO_ERROR) {            \
    using ::the::RuntimeError;                                          \
    using ::the::ui::OglError;                                          \
    if constexpr (std::tuple_size<decltype(                             \
        std::make_tuple(__VA_ARGS__))>::value > 0) {                    \
      std::stringstream ss;                                             \
      ss << OglError{code, PP_WHERE, PP_FUNCTION};                      \
      std::apply([&ss](auto ...x) {                                     \
          (..., (ss << ": " << x));                                     \
        },                                                              \
        std::make_tuple(__VA_ARGS__)                                    \
      );                                                                \
      return {RuntimeError{ss.str()}};                                  \
    } else {                                                            \
      return {OglError{code, PP_WHERE, PP_FUNCTION}};                   \
    }                                                                   \
  }

#define PANIC_ON_GL_ERROR                                               \
  if (auto const code = glGetError(); code != GL_NO_ERROR) {            \
    using ::the::ui::OglError;                                          \
    the::Panic(OglError{code, PP_WHERE, PP_FUNCTION});                  \
  }

namespace the::ui {

struct OglError: public Error {
  constexpr OglError(GLuint const code): code_{code} {}
  constexpr OglError(GLuint const code, char const where[], char const *func)
  : code_{code}, where_{where}, func_{func} {}

  void What(std::ostream &os) const noexcept override {
    if (where_ && func_)
      os << where_ << " `" << func_ << "': ";
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

  GLuint      code_;
  char const *where_ = nullptr;
  char const *func_  = nullptr;
} __attribute__((packed));

template <typename T = std::monostate, typename... Es>
using OglFallible = Fallible<T, OglError, Es...>;

}
