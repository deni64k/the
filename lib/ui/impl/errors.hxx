#pragma once

#include <sstream>
#include <tuple>

#include <GL/glew.h>

#include "lib/utils.hxx"
#include "lib/ui/errors.hxx"

namespace the::ui::impl {

extern char const * GlErrorToString(GLenum err) noexcept;

}

#define FALL_ON_GL_ERROR(...)                                           \
  if (auto const err = glGetError(); err != GL_NO_ERROR) {              \
    std::stringstream ss;                                               \
    ss << __func__ << ':' << __LINE__                                   \
       << ": glGetError=" << err                                        \
       << ": " << ::the::ui::impl::GlErrorToString(err);                \
    if constexpr (std::tuple_size<decltype(                             \
        std::make_tuple(__VA_ARGS__))>::value > 0) {                    \
      std::apply([&ss](auto ...x) {                                     \
          (..., (ss << ": " << x));                                     \
        },                                                              \
        std::make_tuple(__VA_ARGS__)                                    \
      );                                                                \
    }                                                                   \
    return {::the::ui::OpenGlError{ss.str()}};                          \
  }
