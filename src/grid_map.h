#pragma once

#include <optional>
#include <functional>
#include <cassert>
#include <cstdint>
#include "2d_math.h"
#include "geom.h"

template<typename V, size_t W, size_t H>
class array_2d {
private:
  std::array<V, W * H> arr_;

public:
  using reference = V&;
  using const_reference = V const&;


  const_reference getConst(size_t x, size_t y) const {
    assert(x < W);
    assert(y < H);
    return arr_[x + y * W];
  }

  const_reference operator()(size_t x, size_t y) const {
    return getConst(x, y);
  }

  reference get(size_t x, size_t y) {
    if (x >= W) {
      printf("X %zu Y %zu ", x, y);
      exit(-1);
    }
    assert(x < W);
    assert(y < H);
    return arr_[x + y * W];
  }

  reference operator()(size_t x, size_t y) {
    return get(x, y);
  }

  size_t width() const { return W;  }
  size_t height() const { return H; }
};

template<typename V, size_t C, size_t R>
class grid_map {
private:
  array_2d<V, C, R> arr_;
  bounds bounds_;

public:
  using reference = V&;
  using const_reference = V const&;

  grid_map(bounds b) : bounds_(std::move(b)) {}

  vector_2i getIdx(vector_2f v) {
    vector_2f offset = bounds_.clamp(v) - bounds_.min_bound;
    vector_2f cell = (offset * vector_2f{ C, R } / (bounds_.max_bound - bounds_.min_bound)).floor();
    
    return cell.cast<int>().clamp(vector_2i{0, 0}, vector_2i{ C - 1, R - 1 });
  }


  const_reference getConst(vector_2i i) const {
    return arr_.getConst(i.x, i.y);
  }

  const_reference getConst(vector_2f v) const {
    return getConst(getIdx(v));
  }

  const_reference operator()(vector_2i i) const {
    return getConst(i);
  }

  const_reference operator()(vector_2f v) const {
    return getConst(v);
  }

  reference get(vector_2i i) {
    return arr_.get(i.x, i.y);
  }

  reference get(vector_2f v) {
    return get(getIdx(v));
  }

  reference operator()(vector_2i i) {
    return get(i);
  }

  reference operator()(vector_2f v) {
    return get(v);
  }

  bool contains(vector_2f v) const {
    return bounds_.contains(v);
  }

  bool contains(vector_2i i) const {
    // consider not short circuiting
    return (i.x >= 0) && (i.x < C) && (i.y >= 0) && (i.y < R);
  }

  bounds const& getBounds() const { return bounds_; }
  float columns() const { return C; }
  float rows() const { return R; }
};