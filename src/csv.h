#pragma once

#include "utils.h"
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<array>

// this is basically an under-powered C++17 string_view
class buffer_view {
public:
  char const* const c_str;
  size_t const length;
  buffer_view(char const* c_s, int len) : c_str(c_s), length(len) {}
  buffer_view(char const* c_s) : c_str(c_s), length(std::strlen(c_s)) {}

  int int_value() const {
    std::array<char, 64> buffer;
    if (length > (buffer.size() - 1)) { // no room for padded '\0'
      return 0;
    }
    strncpy_s(buffer.data(), buffer.size() - 1, c_str, length);
    buffer[length] = '\0';

    return atoi(buffer.data());
  }

  double double_value() const {
    std::array<char, 64> buffer;
    if (length > (buffer.size() - 1)) { // no room for padded '\0'
      return 0;
    }
    strncpy_s(buffer.data(), buffer.size() - 1, c_str, length);
    buffer[length] = '\0';
    return atof(buffer.data());
  }
  bool operator==(buffer_view const& other) const {
    if (length != other.length) {
      return false;
    }
    return (memcmp(c_str, other.c_str, length) == 0);
  }
};

namespace std {
  template <>
  struct hash<buffer_view> {
    size_t operator()(buffer_view const& k) const {
      // 32 bit FNV-1a hash
      size_t current_hash = 0x811c9dc5;
      for (size_t i = 0; i < k.length; i++) {
        current_hash ^= *(reinterpret_cast<unsigned char const*>(k.c_str + i));
        current_hash *= 16777619;
      }
      return current_hash;
    }
  };

}

class csv {
private:
  int row_count;
  int col_count;
public:
  std::string const data;
  std::vector<std::vector<buffer_view>> const values;
  std::vector<buffer_view> const header;

  csv(std::string str, std::vector<std::vector<buffer_view>> vals, std::vector<buffer_view> head) :
    data(std::move(str)),  values(std::move(vals)), header(std::move(head)) {
    row_count = values.size();
    if (header.size() > 0) {
      col_count = header.size();
    } else if (row_count > 0) {
      col_count = values[0].size();
    } else {
      col_count = 0;
    }
    for (std::vector<buffer_view> const& row : values) {
      assert(row.size() == col_count);
    }
  }

  bool has_header() const {
    return (header.size() == col_count);
  }

  int rows() const {
    return row_count;
  }

  int cols() const {
    return col_count;
  }

  buffer_view const& operator()(int row, int col) const {
    return cell(row, col);
  }

  buffer_view const& cell(int row, int col) const {
    return values[row][col];
  }

  std::string cell_string(int row, int col) const {
    buffer_view const& c = values[row][col];
    return std::string(c.c_str, c.length);
  }

  int get_cell_int(int row, int col) const {
    return cell(row, col).int_value();
  }

  double get_cell_double(int row, int col) const {
    return cell(row, col).double_value();
  }
};

class c_string_reader {
private:
  int remaining;
  char const* position;
  char const* start;
public:

  c_string_reader(char const* c_str) {
    start = c_str;
    position = start;
    remaining = std::strlen(start);
  }

  void advance(int amount) {
    assert(amount <= remaining);
    remaining -= amount;
    position += amount;
  }

  char const* get_start() const {
    return start;
  }

  int get_remaining() const {
    return remaining;
  }

  char const* get_position() const {
    return position;
  }
};

inline std::vector<buffer_view> parse_csv_row(c_string_reader& reader) {
  std::vector<buffer_view> header;
  bool going = true;
  while (going) {
    char const* found = strpbrk(reader.get_position(), ",\n");
    if (found == nullptr) { // none found before EOF
      if (reader.get_remaining() > 0) {
        header.emplace_back(reader.get_position(), reader.get_remaining());
        reader.advance(reader.get_remaining());
      }
      break; // this is also the end of the csv
    } else {
      ptrdiff_t len = found - reader.get_position();
      if ((*found) == ',') {
        header.emplace_back(reader.get_position(), len);
      } else if ((*found) == '\n') {
        header.emplace_back(reader.get_position(), len);
        going = false;
      }
      reader.advance(len + 1);
    }
  }

  return header;
}

inline std::vector<std::vector<buffer_view>> parse_csv_body(c_string_reader &reader) {
  std::vector<std::vector<buffer_view>> values;
  while (reader.get_remaining() > 0) {
    std::vector<buffer_view> row = parse_csv_row(reader);
    if (row.size() != 0) { // skip empty rows
      values.emplace_back(std::move(row));
    }
  }
  return values;
}

inline csv parse_csv(std::string buffer) {
  c_string_reader reader{ buffer.c_str() };
  std::vector<buffer_view> header = parse_csv_row(reader);
  std::vector<std::vector<buffer_view>> values = parse_csv_body(reader);
  return csv(std::move(buffer), std::move(values), std::move(header));
}

inline csv parse_csv_no_header(std::string buffer) {
  c_string_reader reader{ buffer.c_str() };
  std::vector<std::vector<buffer_view>> values = parse_csv_body(reader);
  return csv(std::move(buffer), std::move(values), {});
}

inline csv read_csv(char const* filename) {
  return parse_csv(read_file_to_string(filename));
}

inline csv read_csv_no_header(char const* filename) {
  return parse_csv_no_header(read_file_to_string(filename));

}