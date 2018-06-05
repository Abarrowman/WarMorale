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
    bool divide_x_axis;
  };

  struct search_arg {
    vec_it start;
    vec_it stop;
    bool divide_x_axis;
    kd_node<V>* divider;
  };

  static void consider_for_closests(vector_2f const& pos, std::vector<kd_node_dist<V>>& closests, kd_node<V>& node) {
    if (closests.size() < closests.capacity()) {
      closests.push_back({ node, pos });
      std::push_heap(closests.begin(), closests.end());
    } else {
      kd_node_dist<V> new_node{ node, pos };
      if (closests.front().dist > new_node.dist) {
        std::pop_heap(closests.begin(), closests.end());
        closests.back() = new_node;
        std::push_heap(closests.begin(), closests.end());
      }
    }
  }

  static bool skip_given_closests(std::vector<kd_node_dist<V>>& closests, float min_dist) {
    if (closests.size() < closests.capacity()) {
      return false;
    } else {
      return (min_dist > closests.front().dist);
    }
  }

  static void consider_for_closest(vector_2f const& pos, kd_node_dist<V>& closest, kd_node<V>& node) {
    closest = better_node_dist(closest, {node, pos });
  }

  static bool skip_given_closest(kd_node_dist<V>& closest, float min_dist) {
    return (min_dist > closest.dist);
  }

private: // private varriables

  // The nodes in the kd tree.
  std::vector<kd_node<V>> nodes;

  // The stack used for searching iteratively
  // TODO consider pre-allocating
  std::vector<search_arg> arg_stack_;

private: // private methods

  void inner_build_recursive(vec_it start, vec_it stop, bool divide_x_axis) {
    vec_dst dst = std::distance(start, stop);
    if (dst <= 1) { // sorting 1 or fewer elements need not happen
      return;
    }
    vec_dst median_offset = dst / 2;
    if (divide_x_axis) {
      std::nth_element(start, start + median_offset, stop, compare_kd_node_x);
    } else {
      std::nth_element(start, start + median_offset, stop, compare_kd_node_y);
    }
    inner_build_recursive(start, start + median_offset, !divide_x_axis); // [start, median)
    inner_build_recursive(start + (median_offset + 1), stop, !divide_x_axis); // (median, stop)
  }

  template<typename closest_type, typename skip_func_type, skip_func_type skip_func, typename consider_func_type, consider_func_type consider_func>
  void inner_find_closest_recursive(vector_2f const& pos, vec_it start, vec_it stop, bool divide_x_axis, closest_type& closest) {
    vec_dst dst = std::distance(start, stop);
    if (dst <= 0) {
      return;
    } else if (dst == 1) {
      consider_func(pos, closest, *start);
      return;
    }

    vec_dst median_offset = dst / 2;
    kd_node<V>& median_node = *(start + median_offset);
    bool condition = divide_x_axis ? (pos.x < median_node.pos.x) : (pos.y < median_node.pos.y);

    if (condition) {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (pos, start, start + median_offset, !divide_x_axis, closest);
    } else {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (pos, start + median_offset + 1, stop, !divide_x_axis, closest);
    }

    float min_dist = divide_x_axis ? fabs(pos.x - median_node.pos.x) : fabs(pos.y - median_node.pos.y);
    if (skip_func(closest, min_dist)) {
      return; // this branch can not be closer than the closest so far
    }

    consider_func(pos, closest, median_node);

    if (condition) {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (pos, start + median_offset + 1, stop, !divide_x_axis, closest);
    } else {
      inner_find_closest_recursive<closest_type, skip_func_type, skip_func, consider_func_type, consider_func>
        (pos, start, start + median_offset, !divide_x_axis, closest);
    }
  }

  template<typename closest_type, typename skip_func_type, skip_func_type skip_func, typename consider_func_type, consider_func_type consider_func>
  void inner_find_closest_itreative(vector_2f const& pos, closest_type& closest) {
    // not needed since the while loop always empties arg_stack_
    //arg_stack_.clear();
    
    arg_stack_.push_back({ nodes.begin(), nodes.end(), true, nullptr });

    while (!arg_stack_.empty()) {
      search_arg args = arg_stack_.back();
      arg_stack_.pop_back();
      vec_it start = args.start;
      vec_it stop = args.stop;
      bool divide_x_axis = args.divide_x_axis;

      kd_node<V>* divider = args.divider;

      if (divider != nullptr) {
        // Use the negation of divide_x_axis to reason about the previous branch
        float min_dist = (!divide_x_axis) ? fabs(pos.x - divider->pos.x) : fabs(pos.y - divider->pos.y);
        if (skip_func(closest, min_dist)) {
          continue; // this branch can not be closer than the closest so far
        }
        consider_func(pos, closest, *divider);
      }

      vec_dst dst = std::distance(start, stop);
      if (dst <= 0) {
        continue; // empty sub array
      } else if (dst == 1) {
        consider_func(pos, closest, *start);
        continue;
      }

      vec_dst median_offset = dst / 2;
      kd_node<V>& median_node = *(start + median_offset);
      bool condition = divide_x_axis ? (pos.x < median_node.pos.x) : (pos.y < median_node.pos.y);

      // put the branch not taken on the bellow the branch taken on the stack
      if (condition) {
        arg_stack_.push_back({ start + median_offset + 1, stop, !divide_x_axis, &median_node });
        arg_stack_.push_back({ start, start + median_offset, !divide_x_axis, nullptr });
      } else {
        arg_stack_.push_back({ start, start + median_offset, !divide_x_axis, &median_node });
        arg_stack_.push_back({ start + median_offset + 1, stop, !divide_x_axis, nullptr });
      }

    }
  }

public: // public methods

  kd_tree() {}

  kd_tree(std::vector<kd_node<V>> new_nodes) {
    build_iterative(std::move(new_nodes));
  }


  void build_recursive(std::vector<kd_node<V>> new_nodes) {
    nodes = std::move(new_nodes);
    inner_build_recursive(nodes.begin(), nodes.end(), true);
  }

  void build_iterative(std::vector<kd_node<V>> new_nodes) {
    nodes = std::move(new_nodes);

    // The stack used for building the kd tree.
    // TODO consider putting in class
    // TODO consider pre-allocating
    std::vector<build_arg> arg_stack;

    arg_stack.push_back({ nodes.begin(), nodes.end(), true });
    while (!arg_stack.empty()) {
      build_arg args = arg_stack.back();
      arg_stack.pop_back();
      vec_it start = args.start;
      vec_it stop = args.stop;
      bool divide_x_axis = args.divide_x_axis;
      vec_dst dst = std::distance(start, stop);
      if (dst <= 1) { // sorting 1 or fewer elements need not happen
        continue;
      }
      vec_dst median_offset = dst / 2;
      if (divide_x_axis) {
        std::nth_element(start, start + median_offset, stop, compare_kd_node_x);
      } else {
        std::nth_element(start, start + median_offset, stop, compare_kd_node_y);
      }
      if (dst > 3) { // otherwise both sub arrays will havve 1 or fewer elements
        arg_stack.push_back({ start, start + median_offset, !divide_x_axis }); // [start, median)
        if (dst > 4) { // otherwise 1 or fewer elements will be in right the sub array
          arg_stack.push_back({ start + (median_offset + 1), stop, !divide_x_axis }); // (median, stop)
        }
      }
    }
  }

  // O(n) brute force nearest neighbour
  kd_node<V>* find_closest_brute(vector_2f const& pos) {
    if (nodes.size() == 0) {
      return nullptr;
    }

    kd_node_dist<V> closest{};
    for (kd_node<V>& node : nodes) {
      consider_for_closest(pos, closest, node);
;    }
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_brute(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    if (nodes.size() == 0) {
      return closests;
    }
    closests.reserve(count);
    for (kd_node<V>& node : nodes) {
      consider_for_closests(pos, closests, node);
    }
    std::sort(closests.begin(), closests.end());
    return closests;
  }

  kd_node<V>* find_closest_recursive(vector_2f const& pos) {
    kd_node_dist<V> closest{};
    inner_find_closest_recursive<decltype(closest),
      decltype(skip_given_closest), skip_given_closest,
      decltype(consider_for_closest), consider_for_closest>
      (pos, nodes.begin(), nodes.end(), true, closest);
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_recursive(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    if (nodes.size() == 0) {
      return closests;
    }
    closests.reserve(count);
    inner_find_closest_recursive<decltype(closests),
      decltype(skip_given_closests), skip_given_closests,
      decltype(consider_for_closests), consider_for_closests>
      (pos, nodes.begin(), nodes.end(), true, closests);
    std::sort(closests.begin(), closests.end());
    return closests;
  }

  kd_node<V>* find_closest_iterative(vector_2f const& pos) {
    if (nodes.size() == 0) {
      return nullptr;
    }
    kd_node_dist<V> closest{};
    inner_find_closest_itreative<decltype(closest),
      decltype(skip_given_closest), skip_given_closest,
      decltype(consider_for_closest), consider_for_closest>(pos, closest);
    return closest.ptr;
  }

  std::vector<kd_node_dist<V>> find_count_closest_iterative(vector_2f const& pos, size_t count) {
    std::vector<kd_node_dist<V>> closests;
    if (nodes.size() == 0) {
      return closests;
    }
    closests.reserve(count);
    inner_find_closest_itreative<decltype(closests),
      decltype(skip_given_closests), skip_given_closests,
      decltype(consider_for_closests), consider_for_closests>(pos, closests);
    std::sort(closests.begin(), closests.end());
    return closests;
  }

};