#pragma once

#include <unordered_map>
#include <2d_math.h>

template <typename T>
class space_buckets {
private:
  int cell_size;
  std::unordered_multimap<vector_2i, T> map;
  using map_type = decltype(map);
  using const_iterator = typename map_type::const_iterator;
public:
  using const_range = std::pair<const_iterator, const_iterator>;

private:

  const_range find_local_bucket(vector_2i const& bucket) const {
    return map.equal_range(bucket);
  }

  std::vector<T> find_nearby_buckets(vector_2i const& bucket) const {
    std::vector<T> vec;
    for (int ix = -1; ix < 2; ix++) {
      for (int iy = -1; iy < 2; iy++) {
        auto range = find_local_bucket(bucket + vector_2i(ix, iy));
        for (auto it = range.first; it != range.second; ++it) {
          vec.push_back(it->second);
        }
      }
    }
    return vec;
  }

public:

  vector_2i compute_bucket(vector_2f const& pos) const {
    return ((1.0f / cell_size) * pos).round().cast<int>();
  }

  space_buckets(int cs) : cell_size(cs) {}

  void add_entry(vector_2f const& pos, T const& entry) {
    map.emplace(compute_bucket(pos), entry);
  }

  const_range find_local_bucket(vector_2f const& pos) const {
    return find_local_bucket(compute_bucket(pos));
  }

  std::vector<T> find_nearby_buckets(vector_2f const& pos) const {
    return find_nearby_buckets(compute_bucket(pos));
  }

  void remove_entry(vector_2f const& pos, T const& entry) {
    const_range range = find_local_bucket(pos);
    for (auto it = range.first; it != range.second; ++it) {
      if (it->second == entry) {
        map.erase(it);
        return;
      }
    }
  }

  void move_entry(vector_2f const& old_pos, vector_2f const& new_pos, T const& entry) {
    remove_entry(old_pos, entry);
    add_entry(new_pos, entry);
  }
};

