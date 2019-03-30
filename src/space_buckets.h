#pragma once

#include <array>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "2d_math.h"
#include "sized_vector.h"

#include "grid_map.h"

template <typename T, size_t C, size_t R>
class space_buckets {
public:
  using bucket = std::vector<T>;
  using bucket_ptr = bucket*;
  using bucket_const_ptr = bucket const*;
private:

  using map_type = grid_map<bucket, C, R>;
  map_type cell_map_;

public:

  space_buckets(bounds b) : cell_map_(std::move(b)) {}

  bool location_contains(vector_2f const& pos, T const& entry) {
    bucket& vec cell_map_(pos);
    return (std::find(vec.begin(), vec.end(), entry) != vec.end());
  }

  void add_entry(vector_2f const& pos, T const& entry) {
    cell_map_(pos).push_back(entry);
  }

  bucket_ptr find_bucket(vector_2f const& pos) {
    return &(cell_map_(pos));
  }

  sized_vector<bucket_ptr, 9> find_adj_buckets(vector_2f const& pos) {
    vector_2i idx = cell_map_.getIdx(pos);
    sized_vector<bucket_ptr, 9> nearby;
    for (int iy = -1; iy < 2; iy++) {
      for (int ix = -1; ix < 2; ix++) {

        vector_2i offset_idx = idx + vector_2i(ix, iy);
        if (cell_map_.contains(offset_idx)) {

          bucket& buck = cell_map_.get(offset_idx);
          if (!buck.empty()) {
            nearby.push_back(&buck);
          }
        }
      }
    }
    return nearby;
  }

  void remove_entry(vector_2f const& pos, T const& entry) {
    bucket& vec = cell_map_(pos);
    auto vec_it = std::find(vec.begin(), vec.end(), entry);
    if (vec_it != vec.end()) {
      vec.erase(vec_it);
    }
  }

  void move_entry(vector_2f const& old_pos, vector_2f const& new_pos, T const& entry) {
    remove_entry(old_pos, entry);
    add_entry(new_pos, entry);
  }

  void clear() {
    for (int r = 0; r < R; ++r) {
      for (int c = 0; c < C; ++c) {
        cell_map_.get(vector_2i{ c, r }).clear();
      }
    }
  }
};