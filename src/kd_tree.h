#pragma once

#include "2d_math.h"
#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>

class kd_node {
public:
  vector_2f pos;
  char value;
};

class kd_tree {
private:

  struct kd_best_node {
    kd_node * ptr;
    float dist;
  };

  static bool compare_kd_node_x(kd_node const& left, kd_node const& right) {
    return left.pos.x < right.pos.x;
  }

  static bool compare_kd_node_y(kd_node const& left, kd_node const& right) {
    return left.pos.y < right.pos.y;
  }

  static kd_best_node compare_best_nodes(kd_best_node left, kd_best_node right) {
    if (left.dist < right.dist) {
      return left;
    } else {
      return right;
    }
  }

  static kd_best_node check_compare_best_nodes(kd_best_node ok, kd_best_node maybe_null) {
    if (maybe_null.ptr == nullptr) {
      return ok;
    } else if (ok.dist < maybe_null.dist) {
      return ok;
    } else {
      return maybe_null;
    }
  }

  using vec_it = std::vector<kd_node>::iterator;
  using vec_dst = std::vector<kd_node>::difference_type;
  std::vector<kd_node> nodes;

  struct build_args {
    vec_it start;
    vec_it stop;
    bool divide_x_axis;
  };

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

  float node_distance(vector_2f const& pos, kd_node const& node) {
    return (pos - node.pos).magnitude();
  }

  kd_best_node inner_find_closest_recursive(vector_2f const& pos, vec_it start, vec_it stop, bool divide_x_axis) {
    vec_dst dst = std::distance(start, stop);
    if (dst <= 0) {
      return {nullptr, 0};
    } else if (dst == 1) {
      kd_node& node = *start;
      return { &node, node_distance(pos, node) };
    }

    vec_dst median_offset = dst / 2;
    kd_node& median_node = *(start + median_offset);
    bool condition = divide_x_axis ? (pos.x < median_node.pos.x) : (pos.y < median_node.pos.y);

    kd_best_node first_res = condition ?
      inner_find_closest_recursive(pos, start, start + median_offset, !divide_x_axis) :
      inner_find_closest_recursive(pos, start + median_offset + 1, stop, !divide_x_axis);

    if (first_res.ptr == nullptr) {
      // nothing was found on the path we went down
      kd_best_node second_res = (!condition) ?
        inner_find_closest_recursive(pos, start, start + median_offset, !divide_x_axis) :
        inner_find_closest_recursive(pos, start + median_offset + 1, stop, !divide_x_axis);

      kd_best_node median_res = { &median_node, node_distance(pos, median_node) };
      return check_compare_best_nodes(median_res, second_res);
    } else {
      // something was found on the path we went down
      float min_dist = divide_x_axis ? fabs(pos.x - median_node.pos.x) : fabs(pos.y - median_node.pos.y);
      if (min_dist > first_res.dist) {
        // nothing on the other side of the division could be closer
        return first_res;
      } else {
        //something on the other side could be closer
        kd_best_node second_res = (!condition) ?
          inner_find_closest_recursive(pos, start, start + median_offset, !divide_x_axis) :
          inner_find_closest_recursive(pos, start + median_offset + 1, stop, !divide_x_axis);
        
        kd_best_node median_res = { &median_node, node_distance(pos, median_node) };
        return check_compare_best_nodes(compare_best_nodes(median_res, first_res), second_res);
      }
    }
  }


public:

  void build_recursive(std::vector<kd_node> new_nodes) {
    nodes = std::move(new_nodes);
    inner_build_recursive(nodes.begin(), nodes.end(), true);
  }

  void build_iterative(std::vector<kd_node> new_nodes) {
    nodes = std::move(new_nodes);
    std::vector<build_args> arg_stack;
    // TODO pre-allocate
    arg_stack.push_back({ nodes.begin(), nodes.end(), true });
    while (!arg_stack.empty()) {
      build_args args = arg_stack.back();
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
  kd_node* find_closest_brute(vector_2f const& pos) {
    if (nodes.size() == 0) {
      return nullptr;
    }
    kd_node* best_node = nullptr;
    float best_distance = std::numeric_limits<float>::max();
    for (kd_node& node : nodes) {
      float node_distance = (pos - node.pos).magnitude();
      if (node_distance < best_distance) {
        best_node = &node;
        best_distance = node_distance;
      }
    }
    return best_node;
  }

  kd_node* find_closest_recursive(vector_2f const& pos) {
    kd_best_node best = inner_find_closest_recursive(pos, nodes.begin(), nodes.end(), true);
    return best.ptr;
  }


};