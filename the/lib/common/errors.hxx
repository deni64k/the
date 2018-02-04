#pragma once

#include <variant>
#include <utility>

#include "common.hxx"

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
} __attribute__ ((packed));

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

/*
namespace details {

template <typename T, bool = std::is_literal_type_v<T>>
struct Uninitialized;

template <typename T>
struct Uninitialized<T, true> {
  using ValueType = T;
  
  template <typename ...Args>
  constexpr Unnitialized(InPlaceIndexT<0>, Args && ...args)
  : value_{std::forward<Args>(args)...} {}

  constexpr ValueType const & Get() const & {
    return value_;
  }
  constexpr ValueType & Get() & {
    return value_;
  }
  constexpr ValueType const && Get() const && {
    return std::move(value_);
  }
  constexpr ValueType && Get() && {
    return std::move(value_);
  }

  ValueType value_;
};

template <typename T>
struct Uninitialized<T, false> {
  using ValueType = T;
  
  template <typename ...Args>
  constexpr Unnitialized(InPlaceIndexT<0>, Args && ...args) {
    ::new (&value_) ValueType{std::forward<Args>(args)...};
  }

  constexpr ValueType const & Get() const & {
    return *value_._M_ptr();
  }
  constexpr ValueType & Get() & {
    return *value_._M_ptr();
  }
  constexpr ValueType const && Get() const && {
    return std::move(*value_._M_ptr());
  }
  constexpr ValueType && Get() && {
    return std::move(*value_._M_ptr());
  }

  // TODO: Portable way?
  __gnu_cxx::__aligned_membuf<ValueType> value_;
};

struct InPlaceT {
  explicit InPlaceT() = default;
};
inline constexpr InPlaceT InPlace{};

template <typename T>
struct InPlaceTypeT {
  explicit InPlaceTypeT() = default;
};
template <typename T>
inline constexpr InPlaceTypeT<T> InPlaceType{};

template <std::size_t Index>
struct InPlaceIndexT {
  explicit InPlaceIndexT() = default;
};
template <std::size_t Index>
inline constexpr InPlaceIndexT<Index> InPlaceIndex{};

template <typename ...Ts>
union VariadicUnion {};

template <typename T, typename ...Ts>
union VariadicUnion<T, ...Ts> {
  constexpr VariadicUnion(): ts_{} {}

  template <typename ...Args>
  constexpr VariadicUnion(InPlaceIndexT<0>, Args && ...args)
  : t{InPlaceIndex<0, std::forward<Args>(args)...} {}

  template <std::size_t Index, typename ...Args>
  constexpr VariadicUnion(InPlaceIndexT<Index>, Args && ...args)
  : ts{InPlaceIndex<Index - 1>, std::forward<Args>(args)...} {}

  template <std::size_t I> constexpr T const & Get() const & { return ts.Get<I-1>(); }
  template <std::size_t I> constexpr T & Get() & { return ts.Get<I-1>(); }
  template <> constexpr T const & Get<0>() const & { return t.Get(); }
  template <> constexpr T & Get<0>() & { return t.Get(); }

  template <std::size_t I> constexpr T const && Get() const && { return std::move(ts.Get<I-1>()); }
  template <std::size_t I> constexpr T && Get() && { return std::move(ts.Get<I-1>()); }
  template <> constexpr T const && Get<0>() const && { return std::move(t.Get()); }
  template <> constexpr T && Get<0>() && { return std::move(t.Get()); }

  template <typename U> constexpr U const & Get() const & { return ts.Get<U>(); }
  template <typename U> constexpr U & Get() & { return ts.Get<U>(); }
  template <> constexpr T const & Get() const & { return t.Get(); }
  template <> constexpr T & Get() & { return t.Get(); }

  template <typename U> constexpr U const & Get() const & { return std::move(ts.Get<U>()); }
  template <typename U> constexpr U & Get() & { return std::move(ts.Get<U>()); }
  template <> constexpr T const & Get() const & { return std::move(t.Get()); }
  template <> constexpr T & Get() & { return std::move(t.Get()); }

  Uninitialized<T> t;
  VariadicUnion<Ts...> ts;
};

template <typename T, typename ...Es>
union FallibleStorage {
  friend struct Fallible<T, Es...>;

  using ValueType = T;
  using ErrorType = VariadicUnion<Es...>;

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

}
*/

template <typename T, typename ...Es>
struct [[nodiscard]] FallibleBase final {
  using ValueType = T;
  using ErrorTypes = std::tuple<Es...>;
  using ErrorBaseType = Error;

  constexpr FallibleBase() noexcept: storage_{ValueType{}} {}
  // constexpr FallibleBase(FallibleBase const &) = delete;
  constexpr FallibleBase(FallibleBase const &other) noexcept: storage_{other.storage_} {}
  constexpr FallibleBase(FallibleBase &&other) noexcept: storage_{std::move(other.storage_)} {}
  constexpr FallibleBase(ValueType const &v) noexcept: storage_{v} {}
  constexpr FallibleBase(ValueType &&v) noexcept: storage_{std::forward<ValueType>(v)} {}

  template <typename U> constexpr FallibleBase(U &&u) noexcept
  requires (std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, Es>::value || ...)
  : storage_{std::forward<U>(u)} {}

  operator bool () const {
    return std::holds_alternative<ValueType>(storage_);
  }

  ErrorBaseType const & Err() const & { return std::get<1>(storage_); }
  ErrorBaseType & Err() & { return std::get<1>(storage_); }
  ErrorBaseType const && Err() const && { return std::move(std::get<1>(storage_)); }
  ErrorBaseType && Err() && { return std::move(std::get<1>(storage_)); }

  ValueType *       operator -> ()       { return &std::get<0>(storage_); }
  ValueType const * operator -> () const { return &std::get<0>(storage_); }
  ValueType &       operator *  () &       { return std::get<0>(storage_); }
  ValueType const & operator *  () const & { return std::get<0>(storage_); }
  
  // std::pair<ValueType *, ErrorType *> Lift() {
  //   return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  // }
  // std::pair<ValueType const *, ErrorType *> Lift() const {
  //   return std::make_pair(std::get_if<0>(this), std::get_if<1>(this));
  // }

  // template <typename ...Ess>
  operator FallibleBase<std::monostate, Es...> () const &
  requires (!std::is_same<ValueType, std::monostate>::value) {
    using RetType = FallibleBase<std::monostate, Es...>;
    return std::visit([](auto const &arg) -> decltype(auto) {
        using Arg = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<Arg, ValueType>) {
          return RetType{};
        } else {
          return RetType{arg};
        }
      }, storage_);
  }
  // template <typename ...Ess>
  operator FallibleBase<std::monostate, Es...> () &
  requires (!std::is_same<ValueType, std::monostate>::value) {
    using RetType = FallibleBase<std::monostate, Es...>;
    return std::visit([](auto &arg) -> decltype(auto) {
        using Arg = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<Arg, ValueType>) {
          return RetType{};
        } else {
          return RetType{arg};
        }
      }, storage_);
  }

  // operator FallibleBase<std::monostate, Es...> && () &&
  // requires (!std::is_same<T, std::monostate>::value) {
  //   if (!*this)
  //     return {std::move(*this)};

  //   return std::move(FallibleBase<std::monostate, Es...>{});
  // }

 private:
  std::variant<ValueType, Es...> storage_;
};

template <typename ...Es>
struct [[nodiscard]] FallibleBase<std::monostate, Es...> final {
  using ErrorTypes = std::tuple<Es...>;
  using ErrorBaseType = Error;

  constexpr FallibleBase(): storage_{} {}
  // constexpr FallibleBase(FallibleBase const &) = delete;
  constexpr FallibleBase(FallibleBase const &other) noexcept: storage_{other.storage_} {}
  constexpr FallibleBase(FallibleBase &&other) noexcept: storage_{std::move(other.storage_)} {}
  template <typename U>
  constexpr FallibleBase(U &&u) noexcept
  requires (std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, Es>::value || ...)
  : storage_{std::forward<U>(u)} {}

  operator bool () const {
    return std::holds_alternative<std::monostate>(storage_);
  }

  ErrorBaseType const & Err() const & { return std::get<1>(storage_); }
  ErrorBaseType & Err() & { return std::get<1>(storage_); }

 private:
  std::variant<std::monostate, Es...> storage_;
};

template <typename Value = std::monostate, typename ...Errors>
using Fallible = FallibleBase<Value, RuntimeError, Errors...>;

// template <> explicit Fallible() -> Fallible<void, Error>;
// template <typename T> explicit Fallible(T &&) -> Fallible<T, Error>;
// template <typename T, typename E> explicit Fallible(E &&) -> Fallible<T, E>;

[[noreturn]]
void Panic(Error const &err) noexcept;

}
