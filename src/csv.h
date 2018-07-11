#pragma once

#include "gl_includes.h"
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<array>
#include<cassert>
#include<string_view>
#include<charconv>

class csv {
private:
  int row_count;
  int col_count;
public:
  std::string const data;
  std::vector<std::vector<std::string_view>> const values;
  std::vector<std::string_view> const header;

  csv(std::string str, std::vector<std::vector<std::string_view>> vals, std::vector<std::string_view> head) :
    data(std::move(str)),  values(std::move(vals)), header(std::move(head)) {
    row_count = values.size();
    if (header.size() > 0) {
      col_count = header.size();
    } else if (row_count > 0) {
      col_count = values[0].size();
    } else {
      col_count = 0;
    }
    for (std::vector<std::string_view> const& row : values) {
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

  std::string_view operator()(int row, int col) const {
    return cell(row, col);
  }

  std::string_view cell(int row, int col) const {
    return values[row][col];
  }

  int get_cell_int(int row, int col) const {
    int value;
    std::string_view value_string = cell(row, col);
    std::from_chars(value_string.data(), value_string.data() + value_string.size(), value);
    return value;
  }
};

inline std::vector<std::string_view> parse_csv_row(std::string_view& reader) {
  std::vector<std::string_view> header;
  bool going = true;
  while (going) {
    size_t idx = reader.find_first_of(",\n");
    if (idx == std::string_view::npos) { // none found before EOF
      if (reader.size() > 0) {
        header.emplace_back(reader.data(), reader.size());
        reader = {}; //nothing left to read
      }
      break; // this is also the end of the csv
    } else {
      if (reader[idx] == ',') {
        header.emplace_back(reader.data(), idx);
      } else {
        header.emplace_back(reader.data(), idx);
        going = false;
      }
      reader = reader.substr(idx + 1);
    }
  }

  return header;
}

inline std::vector<std::vector<std::string_view>> parse_csv_body(std::string_view& reader) {
  std::vector<std::vector<std::string_view>> values;
  while (reader.size() > 0) {
    std::vector<std::string_view> row = parse_csv_row(reader);
    if (row.size() != 0) { // skip empty rows
      values.emplace_back(std::move(row));
    }
  }
  return values;
}

inline csv parse_csv(std::string buffer) {
  std::string_view reader = buffer;
  std::vector<std::string_view> header = parse_csv_row(reader);
  std::vector<std::vector<std::string_view>> values = parse_csv_body(reader);
  return csv(std::move(buffer), std::move(values), std::move(header));
}

inline csv parse_csv_no_header(std::string buffer) {
  std::string_view reader = buffer;
  std::vector<std::vector<std::string_view>> values = parse_csv_body(reader);
  return csv(std::move(buffer), std::move(values), {});
}

inline csv read_csv(char const* filename) {
  return parse_csv(read_file_to_string(filename));
}

inline csv read_csv_no_header(char const* filename) {
  return parse_csv_no_header(read_file_to_string(filename));

}