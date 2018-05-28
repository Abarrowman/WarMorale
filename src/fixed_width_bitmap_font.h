#pragma once

#include "texture.h"

class fixed_width_bitmap_font {
public:
  int const char_width;
  int const char_height;
  texture tex;

  fixed_width_bitmap_font(texture t, int c_w, int c_h) : tex(std::move(t)), char_width(c_w), char_height(c_h)  {}

  bool contains_char(char c) const {
    return ((c >= ' ') && (c <= '~'));
  }
};