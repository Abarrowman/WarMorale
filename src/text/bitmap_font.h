#pragma once

#include "../csv.h"
#include "../2d_math.h"
#include "../texture.h"

#include <vector>
#include <unordered_map>
#include <cassert>
#include <charconv>


class prop_bitmap_font {
private:
  std::array<int, 95> char_widths;
  int char_height_;
  int char_max_width_;
  int row_len; // characters per row
  vector_2f char_tex_size_;
public:
  texture tex;

  prop_bitmap_font(texture t, csv info) : tex(std::move(t)) {
    assert(info.cols() == 2);
    std::unordered_map<std::string_view, int> info_pairs;
    for (std::vector<std::string_view> const& row : info.values) {

      int value;
      std::string_view value_string = row[1];
      std::from_chars(value_string.data(), value_string.data() + value_string.size(), value);
      info_pairs[row[0]] = value;
    }
    char_height_ = info_pairs["Cell Height"];
    char_max_width_ = info_pairs["Cell Width"];

    row_len = tex.width() / char_max_width_;

    char_tex_size_.x = static_cast<float>(char_max_width_) / tex.width();
    char_tex_size_.y = static_cast<float>(char_height_) / tex.height();

    std::array<char, 20> base_width_name_buffer;
    for (int i = ' '; i <= '~'; i++) {
      size_t len = snprintf(base_width_name_buffer.data(), base_width_name_buffer.size(),
        "Char %d Base Width", i);
      int char_width = info_pairs[{base_width_name_buffer.data(), len}];
      char_widths[i - ' '] = char_width;
    }
  }

  vector_2f top_left_tex(char c) const {
    assert(contains_char(c));
    int idx = c - ' ';
    return vector_2f{ (idx % row_len) * char_tex_size_.x, (idx / row_len) * char_tex_size_.y };
  }

  bool contains_char(char c) const {
    return ((c >= ' ') && (c <= '~'));
  }

  int char_width(char c) const {
    assert(contains_char(c));
    return char_widths[static_cast<int>(c - ' ')];
  }

  int char_max_width() const {
    return char_max_width_;
  }

  int char_height() const {
    return char_height_;
  }

  vector_2f const& char_tex_size() const {
    return char_tex_size_;
  }


};

class mono_bitmap_font {
public:
  texture tex;
  int const char_width;
  int const char_height;
  int const row_length; // characters per row
  vector_2f const char_tex_size; // size of character in texture coordinates

  mono_bitmap_font(texture t, int c_w, int c_h) :
    tex(std::move(t)),
    char_width(c_w),
    char_height(c_h),
    row_length(tex.width() / char_width),
    char_tex_size(
      static_cast<float>(char_width) / tex.width(),
      static_cast<float>(char_height) / tex.height()) {}

  bool contains_char(char c) const {
    return ((c >= ' ') && (c <= '~'));
  }

  vector_2f top_left_tex(char c) const {
    assert(contains_char(c));
    int idx = c - ' ';
    return vector_2f{ (idx % row_length) * char_tex_size.x, (idx / row_length) * char_tex_size.y };
  }
};
