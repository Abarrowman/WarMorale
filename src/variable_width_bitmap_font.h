#pragma once

#include "texture.h"
#include "csv.h"
#include <unordered_map>
#include <cassert>
#include <charconv>

class variable_width_bitmap_font {
private:
  std::array<int, 95> char_widths;
  int char_high;
  int char_wide;
public:
  texture tex;

  variable_width_bitmap_font(texture t, csv info) : tex(std::move(t)) {
    assert(info.cols() == 2);
    std::unordered_map<std::string_view, int> info_pairs;
    for (std::vector<std::string_view> const& row : info.values) {

      int value;
      std::string_view value_string = row[1];
      std::from_chars(value_string.data(), value_string.data() + value_string.size(), value);
      info_pairs[row[0]] = value;
    }
    char_high = info_pairs["Cell Height"];
    char_wide = info_pairs["Cell Width"];
    std::array<char, 20> base_width_name_buffer;
    for (int i = ' '; i <= '~'; i++) {
      size_t len = snprintf(base_width_name_buffer.data(), base_width_name_buffer.size(), "Char %d Base Width", i);
      int char_width = info_pairs[{base_width_name_buffer.data(), len}];
      char_widths[i - ' '] = char_width;
    }
  }

  /*vector_2f char_texture_top_left_coordinates(char c) const {
    assert(contains_char(c));
    int idx = c - ' ';
    int row_len = tex.width / char_wide;
    int row = idx / row_len;
    int col = idx % row_len;
    return vector_2f((static_cast<float>(col) * char_wide) / tex.width, (static_cast<float>(row) * char_high) / tex.height);
  }*/

  bool contains_char(char c) const {
    return ((c >= ' ') && (c <= '~'));
  }

  int char_width(char c) const {
    assert(contains_char(c));
    return char_widths[static_cast<int>(c - ' ')];
  }

  int char_max_width() const {
    return char_wide;
  }

  int char_height() const {
    return char_high;
  }
};