#pragma once

#include "2d_math.h"
#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>

template<typename V>
struct kd_node {
  vector_2f pos;
  V value;
};

template <typename V>
struct kd_node_dist {
  kd_node<V>* ptr;
  float dist;

  kd_node_dist() : ptr(nullptr), dist(std::numeric_limits<float>::max()) {}
  kd_node_dist(kd_node<V>* pt, float dis) : ptr(pt), dist(dis) {}
  kd_node_dist(kd_node<V>& node, vector_2f const& pos) : ptr(&node), dist((pos - node.pos).magnitude()) {}

  bool operator<(kd_node_dist<V> const& other) {
    return (dist < other.dist);
  }
};

template<typename V>
class kd_tree {
private: // static members
  static int next_division_axis(int division_axis) {
    return (division_axis + 1) % 2;
  }

  static int previous_division_axis(int division_axis) {
    return (division_axis == 0) ? (2 - 1) : (division_axis - 1);
  }

  static bool compare_kd_node_x(kd_node<V> const& left, kd_node<V> const& right) {
    return left.pos.x < right.pos.x;
  }

  static bool compare_kd_node_y(kd_node<V> const& left, kd_node<V> const& right) {
    return left.pos.y < right.pos.y;
  }

  static kd_node_dist<V> better_node_dist(kd_node_dist<V> left, kd_node_dist<V> right) {
    if (left < right) {
      return left;
    } else {
      return right;
    }
  }

  using vec_it = typename std::vector<kd_node<V>>::iterator;
  using vec_dst = typename std::vector<kd_node<V>>::difference_type;

  struct build_arg {
    vec_it start;
    vec_it stop;
    int division_axis;
  };

  struct search_arg {
    vec_it start;
    vec_it stop;
    int division_axis;
    kd_node<V>* divider;
  };

  struct search_ctx {
    vector_2f const& pos;
    float _max_dist;
    search_ctx(vector_2f const& p) : pos(p) {}
    search_ctx(vector_2f const& p, float md) : pos(p), _max_dist(md) {}

  };

  static void consider_for_closests(search_ctx const& ctx, std::vector<kd_node_dist<V>>& closests, kd_node<V>& node) {
    if (closests.size() < closests.capacity()) {
      closests.push_back({ node, ctx.pos });
      std::push_heap(closests.begin(), closests.end());
    } else {
      kd_node_dist<V> new_node{ node, ctx.pos };
      if (closests.front().dist > new_node.dist) {
        std::pop_heap(closests.begin(), closests.end());
        closests.back() = new_node;
        std::push_heap(closests.begin(), closests.end());
      }
    }
  }

  static bool skip_given_closests(search_ctx const& ctx, std::vector<kd_node_dist<V>>& closests, float min_dist) {
    if (closests.size() < closests.capacity()) {
      return false;
    } else {
      return (min_dist > closests.front().dist);
    }
  }

  static void consider_for_closest(search_ctx const& ctx, kd_node_dist<V>& closest, kd_node<V>& node) {
    closest = better_node_dist(closest, {node, ctx.pos });
  }

  static bool skip_given_closest(search_ctx const& ctx, kd_node_dist<V>& closest, float min_dist) {
    return (min_dist > closest.dist);
  }

  static void consider_within_distance(search_ctx const& ctx, std::vector<kd_node_dist<V>>& within, kd_node<V>& node) {
    kd_node_dist<V> new_node{ node, ctx.pos };
    if (new_node.dist < ctx._max_dist) {
      within.push_back(new_node);
    }
  }

  static bool skip_given_distance(search_ctx const& ctx, std::vector<kd_node_dist<V>>& within, float min_dist) {
    return (min_dist > ctx._max_dist);
  }

private: // private varriables

  // The nodes in the kd tree.
  std::vector<kd_node<V>> nodes;

private: // private methods

  void inner_build_recursive(vec_it start, vec_it stop, int division_axis) {
    vec_dst dst = std::distance(start, stop);
    if (dst <= 1) { // sorting 1 or fewer elements need not happen
      return;
    }
    vec_dst median_offset = dst / 2;
    if (division_axis == 0) {
      std::nth_element(start, start + median_offset, stop, compare_kd_node_x);
    } else {
      std::nth_element(start, start + median_offset, stop, compare_kd_node_y);
    }
    inner_build_recursive(start, start + median_offset, next_division_axis(division_axis)); // [start, median)
    inner_build_recursive(start + (median_offset + 1), stop, next_division_axis(division_axis)); // (median, stop)
  }

  template<typename closest_type, typename skip_func_type, skip_func_type skip_func,
    typename consider_func_type, consider_func_type consider_func>
  void inner_find_closest_recursive(search_ctx const& ctx, vec_it start, vec_it stop, int division_axis, closest_type& closest) {
    vec_dst dst = std::distance(start, stop);
    if (dst <= 0) {
      return;
    } else if (dst == 1) {
      consider_func(ctx, closest, *start);
      return;
    }

    vec_dst median_offset = dst / 2;
    kd_node<V>& median_node = *(start + median_offset);
    bool condition = (division_axis == 0) ? (ctx.pos.x < median_node.pos.x) : (ctx.pos.y < median_node.pos.y);

    if (condition) {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (ctx, start, start + median_offset, next_division_axis(division_axis), closest);
    } else {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (ctx, start + median_offset + 1, stop, next_division_axis(division_axis), closest);
    }

    float min_dist = (division_axis == 0) ? fabs(ctx.pos.x - median_node.pos.x) : fabs(ctx.pos.y - median_node.pos.y);
    if (skip_func(ctx, closest, min_dist)) {
      return; // this branch can not be closer than the closest so far
    }

    consider_func(ctx, closest, median_node);

    if (condition) {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (ctx, start + median_offset + 1, stop, next_division_axis(division_axis), closest);
    } else {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (ctx, start, start + median_offset, next_division_axis(division_axis), closest);
    }
  }

  template<typename closest_type, typename skip_func_type, skip_func_type skip_func,
    typename consider_func_type, consider_func_type consider_func>
  void inner_find_closest_iterative(search_ctx const& ctx, closest_type& closest) {

    // TODO consider pre-allocating
    // TODO consider putting in class
    std::vector<search_arg> arg_stack;
    
    arg_stack.push_back({ nodes.begin(), nodes.end(), 0, nullptr });

    while (!arg_stack.empty()) {
      search_arg args = arg_stack.back();
      arg_stack.pop_back();
      vec_it start = args.start;
      vec_it stop = args.stop;
      int division_axis = args.division_axis;
      kd_node<V>* divider = args.divider;

      if (divider != nullptr) {


        float min_dist = (previous_division_axis(division_axis) == 0) ? fabs(ctx.pos.x - divider->pos.x) : fabs(ctx.pos.y - divider->pos.y);
        if (skip_func(ctx, closest, min_dist)) {
          continue; // this branch can not be closer than the closest so far
        }
        consider_func(ctx, closest, *divider);
      }

      vec_dst dst = std::distance(start, stop);
      if (dst <= 0) {
        continue; // empty sub array
      } else if (dst == 1) {
        consider_func(ctx, closest, *start);
        continue;
      }

      vec_dst median_offset = dst / 2;
      kd_node<V>& median_node = *(start + median_offset);
      bool condition = (division_axis == 0) ? (ctx.pos.x < median_node.pos.x) : (ctx.pos.y < median_node.pos.y);

      // put the branch not taken on the bellow the branch taken on the stack
      if (condition) {
        arg_stack.push_back({ start + median_offset + 1, stop, next_division_axis(division_axis), &median_node });
        arg_stack.push_back({ start, start + median_offset, next_division_axis(division_axis), nullptr });
      } else {
        arg_stack.push_back({ start, start + median_offset, next_division_axis(division_axis), &median_node });
        arg_stack.push_back({ start + median_offset + 1, stop, next_division_axis(division_axis), nullptr });
      }

    }
  }

public: // public methods

  kd_tree() {}

  kd_tree(std::vector<kd_node<V>> new_nodes) {
    build_iterative(std::move(new_nodes));
    //build_recursive(std::move(new_nodes));
  }


  void build_recursive(std::vector<kd_node<V>> new_nodes) {
    nodes = std::move(new_nodes);
    inner_build_recursive(nodes.begin(), nodes.end(), 0);
  }

  void build_iterative(std::vector<kd_node<V>> new_nodes) {
    nodes = std::move(new_nodes);

    // The stack used for building the kd tree.
    // TODO consider putting in class
    // TODO consider pre-allocating
    std::vector<build_arg> arg_stack;

    arg_stack.push_back({ nodes.begin(), nodes.end(), 0 });
    while (!arg_stack.empty()) {
      build_arg args = arg_stack.back();
      arg_stack.pop_back();
      vec_it start = args.start;
      vec_it stop = args.stop;
      int division_axis = args.division_axis;
      vec_dst dst = std::distance(start, stop);
      if (dst <= 1) { // sorting 1 or fewer elements need not happen
        continue;
      }
      vec_dst median_offset = dst / 2;
      if (division_axis == 0) {
        std::nth_element(start, start + median_offset, stop, compare_kd_node_x);
      } else {
        std::nth_element(start, start + median_offset, stop, compare_kd_node_y);
      }
      if (dst > 3) { // otherwise both sub arrays will havve 1 or fewer elements
        arg_stack.push_back({ start, start + median_offset, next_division_axis(division_axis) }); // [start, median)
        if (dst > 4) { // otherwise 1 or fewer elements will be in right the sub array
          arg_stack.push_back({ start + (median_offset + 1), stop, next_division_axis(division_axis) }); // (median, stop)
        }
      }
    }
  }

  // O(n) brute force nearest neighbour
  kd_node<V>* find_closest_brute(vector_2f const& pos) {
    kd_node_dist<V> closest{};
    for (kd_node<V>& node : nodes) {
      consider_for_closest(pos, closest, node);
;    }
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_brute(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    closests.reserve(count);
    for (kd_node<V>& node : nodes) {
      consider_for_closests(pos, closests, node);
    }
    std::sort(closests.begin(), closests.end());
    return closests;
  }

  std::vector<kd_node_dist<V>> find_within_brute(vector_2f const& pos, float max_distance) {
    std::vector<kd_node_dist<V>> within;
    search_ctx ctx{ pos, max_distance };
    for (kd_node<V>& node : nodes) {
      consider_within_distance(ctx, within, node);
    }
    std::sort(within.begin(), within.end());
    return within;
  }

  kd_node<V>* find_closest_recursive(vector_2f const& pos) {
    kd_node_dist<V> closest{};
    inner_find_closest_recursive<decltype(closest),
      decltype(skip_given_closest), skip_given_closest,
      decltype(consider_for_closest), consider_for_closest>
      (pos, nodes.begin(), nodes.end(), 0, closest);
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_recursive(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    closests.reserve(count);
    inner_find_closest_recursive<decltype(closests),
      decltype(skip_given_closests), skip_given_closests,
      decltype(consider_for_closests), consider_for_closests>
      (pos, nodes.begin(), nodes.end(), 0, closests);
    std::sort(closests.begin(), closests.end());
    return closests;
  }

  std::vector<kd_node_dist<V>> find_within_recursive(vector_2f const& pos, float max_distance) {
    std::vector<kd_node_dist<V>> within;

    inner_find_closest_recursive<decltype(within),
      decltype(skip_given_distance), skip_given_distance,
      decltype(consider_within_distance), consider_within_distance>
      ({ pos, max_distance }, nodes.begin(), nodes.end(), 0, within);
    std::sort(within.begin(), within.end());
    return within;
  }

  kd_node<V>* find_closest_iterative(vector_2f const& pos) {
    kd_node_dist<V> closest{};
    inner_find_closest_iterative<decltype(closest),
      decltype(skip_given_closest), skip_given_closest,
      decltype(consider_for_closest), consider_for_closest>(pos, closest);
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_iterative(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    closests.reserve(count);
    inner_find_closest_iterative<decltype(closests),
      decltype(skip_given_closests), skip_given_closests,
      decltype(consider_for_closests), consider_for_closests>(pos, closests);
    std::sort(closests.begin(), closests.end());
    return closests;
  }

  std::vector<kd_node_dist<V>> find_within_iterative(vector_2f const& pos, float max_distance) {
    std::vector<kd_node_dist<V>> within;

    inner_find_closest_iterative<decltype(within),
      decltype(skip_given_distance), skip_given_distance,
      decltype(consider_within_distance), consider_within_distance>
      ({ pos, max_distance }, within);
    std::sort(within.begin(), within.end());
    return within;
  }

};