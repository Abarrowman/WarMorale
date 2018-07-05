#pragma once

#include <array>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <2d_math.h>

template <typename T>
class nearby_buckets {
public:

  class iterator {
  private:
    nearby_buckets<T> const& container;
    size_t array_idx;
    size_t vector_idx;
  public:

    iterator(nearby_buckets<T> const& con, size_t ai, size_t vi) : container(con), array_idx(ai), vector_idx(vi) {}

    iterator& operator++() {
      std::vector<T>& vec = *(container.values[array_idx]);
      if ((vector_idx + 1) < vec.size()) { // advance within current vector
        vector_idx++;
      } else { // advance to the next non-empty vector or the end iterator
        vector_idx = 0;
        array_idx++;
        while (array_idx < 9) {
          std::vector<T>* vec_ptr = container.values[array_idx];
          if ((vec_ptr != nullptr) && (vec_ptr->size() > 0)) {
            break;
          } else {
            array_idx++;
          }
        }
      }
      return *this;
    }

    T& operator*() {
      return (*container.values[array_idx])[vector_idx];
    }

    bool operator==(iterator const& other) const {
      return ((&container) == (&(other.container))) &&
        (array_idx == other.array_idx) &&
        (vector_idx == other.vector_idx);
    }

    bool operator!=(iterator const& other) const {
      return !(operator==(other));
    }
  };

  std::array<std::vector<T>*, 9> values;

  iterator begin() const {
    for (size_t ai = 0; ai < 9; ai++) {
      std::vector<T>* vec_ptr = values[ai];
      if ((vec_ptr != nullptr) && (vec_ptr->size() > 0)) {
        return iterator{ *this, ai, 0 };
      }
    }
    return end();
  }

  iterator end() const {
    return iterator{ *this, 9, 0 };
  }

};

template <typename T>
class space_buckets {
private:
  int cell_size;
  std::unordered_map<vector_2i, std::vector<T>> map;
  using map_type = decltype(map);
  using iterator = typename map_type::iterator;
public:
  

  
private:

  std::vector<T>* find_local_bucket(vector_2i const& bucket) {
    iterator it = map.find(bucket);
    if (it == map.end()) {
      return nullptr;
    } else {
      std::vector<T>& vec = it->second;
      return &vec;
    }
  }

  nearby_buckets<T> find_nearby_buckets(vector_2i const& bucket) {
    nearby_buckets<T> near;
    for (int iy = -1; iy < 2; iy++) {
      for (int ix = -1; ix < 2; ix++) {
        int idx = ix + 1 + (iy + 1) * 3;
        near.values[idx] = find_local_bucket(bucket + vector_2i(ix, iy));
      }
    }
    return near;
  }

  bool contains(vector_2i const& bucket, T const& entry) {
    std::vector<T>* vec_ptr = find_local_bucket(bucket);
    if (vec_ptr == nullptr) {
      return false;
    }
    std::vector<T>& vec = *vec_ptr;
    auto it = std::find(vec.begin(), vec.end(), entry);
    if (it == vec.end()) {
      return false;
    } else {
      return true;
    }
  }

  vector_2i compute_bucket(vector_2f const& pos) const {
    return ((1.0f / cell_size) * pos).round().cast<int>();
  }

public:

  bool contains(vector_2f const& pos, T const& entry) {
    return contains(compute_bucket(pos), entry);
  }

  space_buckets(int cs) : cell_size(cs) {}

  void add_entry(vector_2f const& pos, T const& entry) {
    vector_2i const& ibucket = compute_bucket(pos);
    iterator map_it = map.find(ibucket);
    if (map_it == map.end()) {
      std::tie(map_it, std::ignore) = map.emplace( ibucket, std::vector<T>());
    }
    std::vector<T>& vec = map_it->second;
    vec.push_back(entry);
  }

  std::vector<T>* find_local_bucket(vector_2f const& pos) {
    return find_local_bucket(compute_bucket(pos));
  }

  nearby_buckets<T> find_nearby_buckets(vector_2f const& pos) {
    return find_nearby_buckets(compute_bucket(pos));
  }

  void remove_entry(vector_2f const& pos, T const& entry) {
    vector_2i const& ibucket = compute_bucket(pos);

    iterator map_it = map.find(ibucket);
    if (map_it == map.cend()) {
      return;
    }

    std::vector<T>& vec = map_it->second;
    auto vec_it = std::find_if(vec.begin(), vec.end(), [&entry](T& elem) { return elem == entry; });
    if (vec_it != vec.end()) {
      vec.erase(vec_it);
    }
    // TODO keep "leaking" for now
    /*// what if the bucket is now empty
    if (vec.size() == 0) {
      map.erase(map_it);
    }*/
  }

  void move_entry(vector_2f const& old_pos, vector_2f const& new_pos, T const& entry) {
    remove_entry(old_pos, entry);
    add_entry(new_pos, entry);
  }
};