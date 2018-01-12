#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <variant>

// #define _GNU_SOURCE
// #define try
// #define catch(...)
// #include <boost/stacktrace.hpp>
// #undef _GNU_SOURCE
// #undef try
// #undef catch

namespace the {

std::string LoadFile(char const *fpath);

struct Error {
  Error() = delete;
  Error(char const *msg)
      : message_(msg)
      // , stacktrace_(boost::stacktrace::stacktrace())
  {}
  Error(std::string const &msg)
      : message_(msg)
      // , stacktrace_(boost::stacktrace::stacktrace())
  {}
  Error(std::string &&msg)
      : message_(msg)
      // , stacktrace_(boost::stacktrace::stacktrace())
  {}
  virtual ~Error() {}

  virtual char const * What() const noexcept {
    return message_.c_str();
  }

  // virtual boost::stacktrace::stacktrace const & Where() const noexcept {
  //   return stacktrace_;
  // }
  
 private:
  // TODO: std::string and stacktrace requires memory allocation. Find a way to avoid it.
  std::string message_;
  // boost::stacktrace::stacktrace stacktrace_;
};

inline
std::ostream & operator << (std::ostream & os, Error const &err) {
  os << err.What();
  return os;
}

template <typename T = std::monostate>
struct [[nodiscard]] Fallible final: std::variant<T, Error> {
  using Base = std::variant<T, Error>;

  constexpr Fallible() noexcept: Base{T{}} {}
  constexpr Fallible(Fallible const &) = delete;
  constexpr Fallible(Fallible &&other) noexcept: Base{other} {}
  // constexpr Fallible(T &&t) noexcept: Base{t} {}
  // constexpr Fallible(Error &&err) noexcept: Base{err} {}
  template <typename U>
  constexpr Fallible(U &&u) noexcept: Base{std::forward<U>(u)} {}

  operator bool () const {
    return !std::holds_alternative<Error>(*this);
  }

  Error const & Err() const {
    return std::get<1>(*this);
  }

  T *       operator -> ()       { return &std::get<0>(*this); }
  T const * operator -> () const { return &std::get<0>(*this); }
  T &       operator *  ()       { return std::get<0>(*this); }
  T const & operator *  () const { return std::get<0>(*this); }
  
  std::pair<T *, Error *> Lift() {
    return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  }
  std::pair<T const *, Error *> Lift() const {
    return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  }

  operator Fallible<> () const {
    if (!*this)
      return {Err()};

    return {};
  }
};

[[noreturn]]
void Panic(Error const &err) noexcept;

}
