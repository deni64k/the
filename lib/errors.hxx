#pragma once

#include <variant>

// #define _GNU_SOURCE
// #define try
// #define catch(...)
// #include <boost/stacktrace.hpp>
// #undef _GNU_SOURCE
// #undef try
// #undef catch

namespace the {

struct Error {
  virtual ~Error();
  virtual void What(std::ostream &os) const noexcept = 0;
};

std::ostream & operator << (std::ostream &, Error const &);

struct RuntimeError: public Error {
  RuntimeError(char const *msg): message_(msg) {}
  RuntimeError(std::string const &msg): message_(msg) {}
  RuntimeError(std::string &&msg): message_(msg) {}
  virtual ~RuntimeError() = default;

  void What(std::ostream &os) const noexcept override {
    os << message_;
  }
  // virtual boost::stacktrace::stacktrace const & Where() const noexcept;
  
 protected:
  std::string message_;
  // boost::stacktrace::stacktrace stacktrace_;
};

template <typename T, typename E>
union FallibleStorage final {
  friend struct Fallible<T, E>;

  using ValueType = T;
  using ErrorType = E;

  FallibleStorage() {}
  ~FallibleStorage() {}

  void ConstructValue(ValueType const &value) { new (&value_) ValueType(value); }
  void ConstructValue(ValueType &&value) { new (&value_) ValueType(std::move(value)); }
  void DestructValue() { &value_.~ValueType(); }
  constexpr ValueType const & Value() const & { return value_; }
  ValueType & Value() & { return value_; }
  ValueType && Value() && { return std::move(value_); }

  void ConstructError(ErrorType const &error) { new (&error_) ErrorType(error); }
  void ConstructError(ErrorType &&error) { new (&error_) ErrorType(std::move(error)); }
  void DestructError() { &error_.~ErrorType(); }
  constexpr ErrorType const & Error() const { return error_; }
  ErrorType & Error() { return error_; }

 private:
  ValueType value_;
  ErrorType error_;
};

template <typename E>
union FallibleStorage<void, E> final {
  friend struct Fallible<void, E>;

  using ValueType = void;
  using ErrorType = E;

  FallibleStorage() {}
  ~FallibleStorage() {}

  void ConstructError(ErrorType const &error) { new (&error_) ErrorType(error); }
  void ConstructError(ErrorType &&error) { new (&error_) ErrorType(std::move(error)); }
  void DestructError() { &error_.~ErrorType(); }
  constexpr ErrorType const & Error() const { return error_; }
  ErrorType & Error() { return error_; }

 private:
  ErrorType error_;
};

template <typename T, typename E = Error>
struct [[nodiscard]] FallibleStorage final {
  using ValueType = T;
  using ErrorType = E;

  constexpr Fallible() noexcept = delete;
  constexpr Fallible(Fallible const &) = delete;
  constexpr Fallible(Fallible &&other) noexcept: BaseType{other} {}
  template <typename U>
  constexpr Fallible(U &&u) noexcept: BaseType{std::forward<U>(u)} {}

  operator bool () const {
    return std::holds_alternative<ValueType>(*this);
  }

  ErrorType const & Err() const {
    return std::get<1>(*this);
  }

  ValueType *       operator -> ()       { return &std::get<0>(*this); }
  ValueType const * operator -> () const { return &std::get<0>(*this); }
  ValueType &       operator *  ()       { return std::get<0>(*this); }
  ValueType const & operator *  () const { return std::get<0>(*this); }
  
  std::pair<ValueType *, ErrorType *> Lift() {
    return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  }
  std::pair<ValueType const *, ErrorType *> Lift() const {
    return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  }

  operator Fallible<void, ErrorType> () const {
    if (!*this)
      return {Err()};

    return {};
  }

 private:
  FallibleStorage<ValueType, ErrorType> storage_;
};

// template <> explicit Fallible() -> Fallible<void, Error>;
template <typename T> explicit Fallible(T &&) -> Fallible<T, Error>;
template <typename T, typename E> explicit Fallible(E &&) -> Fallible<T, E>;

[[noreturn]]
void Panic(Error const &err) noexcept;

}
