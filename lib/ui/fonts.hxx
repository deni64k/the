#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <gsl.h>

#include "lib/utils.hxx"

namespace astro::ui {

template <typename T>
struct Image {

  struct ColumnAdapter {
    ColumnAdapter() = delete;
    ColumnAdapter(Image &image, std::size_t col)
        : image_(image)
        , col_(col)
    {}

    T & operator [] (std::size_t row) {
      return *(image_.data() + row * image_.Width() + col_);
    }
    T const & operator [] (std::size_t row) const {
      return *(image_.data() + row * image_.Width() + col_);
    }

   private:
    Image &image_;
    std::size_t col_;
  };

  struct ColumnAdapterConst {
    ColumnAdapterConst() = delete;
    ColumnAdapterConst(Image const &image, std::size_t col)
        : image_(image)
        , col_(col)
    {}

    T const & operator [] (std::size_t row) const {
      return *(image_.data() + row * image_.Width() + col_);
    }

   private:
    Image const &image_;
    std::size_t col_;
  };
  
  Image(std::size_t width, std::size_t height)
      : data_{std::make_unique<T[]>(width * height)}
      , width_{width}
      , height_{height}
  {}
  Image(Image &&other)
      : data_{std::move(other.data_)}
      , width_{other.width_}
      , height_{other.height_}
  {}

  ColumnAdapter      operator [] (std::size_t row)       { return {*this, row}; }
  ColumnAdapterConst operator [] (std::size_t row) const { return {*this, row}; }

  std::size_t Width()  const { return width_; }
  std::size_t Height() const { return height_; }

  // STL interface methods
  std::size_t size()   const { return width_ * height_; }

  T *       begin()        { return data_.get(); }
  T const * begin()  const { return data_.get(); }
  T const * cbegin() const { return data_.get(); }

  T *       end()        { return data_.get() + size(); }
  T const * end()  const { return data_.get() + size(); }
  T const * cend() const { return data_.get() + size(); }

  T *       data()        { return data_.get(); }
  T const * data()  const { return data_.get(); }

  void fill(T const color) {
    std::fill(data_.get(), end(), color);
  }
  
 private:
  std::unique_ptr<T[]> data_;
  std::size_t width_, height_;
};

Fallible<Image<std::uint8_t[4]>> RenderFont(gsl::span<char const> text);

}
