#pragma once
#include "../kd_tree.h"

// TODO integrate a unit testing framework

inline void test_kd_tree() {
  kd_tree<char> kd{ {
    { { 2, 3 }, 'a' },
  { { 5, 4 }, 'b' },
  { { 9, 6 }, 'c' },
  { { 4, 7 }, 'd' },
  { { 8, 1 }, 'e' },
  { { 7, 2 }, 'f' },
    } };

  /*std::vector<kd_node<char>> nodes;
  int node_count = 1000;
  for (int n = 0; n < node_count; n++) {
  nodes.push_back({{ rand_float(gen) * 10.0f , rand_float(gen)  * 10.0f }, static_cast<char>(n)});
  }
  kd_tree<char> kd{ std::move(nodes) };*/

  for (float x = -1; x < 11; x += 0.1f) {
    for (float y = -1; y < 11; y += 0.1f) {
      vector_2f pos{ x, y };
      kd_node<char>* close_rec = kd.find_closest_recursive(pos);
      kd_node<char>* close_brute = kd.find_closest_brute(pos);
      kd_node<char>* close_it = kd.find_closest_iterative(pos);

      if (close_rec != close_brute) {
        float dis_brute = (close_brute->pos - pos).magnitude();
        float dis_rec = (close_rec->pos - pos).magnitude();
        if (dis_brute < dis_rec) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }
      }
      if (close_it != close_brute) {
        float dis_brute = (close_brute->pos - pos).magnitude();
        float dis_it = (close_it->pos - pos).magnitude();
        if (dis_brute < dis_it) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-2);
        }
      }

      size_t count = 3;
      std::vector<kd_node_dist<char>> closests_brute = kd.find_count_closest_brute(pos, count);
      if (closests_brute.size() != count) {
        fprintf(stderr, "Error at (%f, %f)\n", x, y);
        exit(-3);
      }

      if (closests_brute[0].ptr != close_brute) {
        float dis_brute = (close_brute->pos - pos).magnitude();
        if (dis_brute < closests_brute[0].dist) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-4);
        }
      }

      std::vector<kd_node_dist<char>> closests_rec = kd.find_count_closest_recursive(pos, count);
      if (closests_rec.size() != count) {
        fprintf(stderr, "Error at (%f, %f)\n", x, y);
        exit(-5);
      }
      for (size_t i = 0; i < count; i++) {
        if (closests_brute[i].ptr != closests_rec[i].ptr) {
          if (closests_brute[i].dist < closests_rec[i].dist) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-6);
          }
        }
      }

      std::vector<kd_node_dist<char>> closests_it = kd.find_count_closest_iterative(pos, count);
      if (closests_it.size() != count) {
        fprintf(stderr, "Error at (%f, %f)\n", x, y);
        exit(-7);
      }
      for (size_t i = 0; i < count; i++) {
        if (closests_brute[i].ptr != closests_it[i].ptr) {
          if (closests_brute[i].dist < closests_it[i].dist) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-8);
          }
        }
      }

      std::vector<kd_node_dist<char>> within_brtue = kd.find_within_brute(pos, 3.0f);
      std::vector<kd_node_dist<char>> within_rec = kd.find_within_recursive(pos, 3.0f);
      if (within_rec.size() != within_brtue.size()) {
        fprintf(stderr, "Error at (%f, %f)\n", x, y);
        exit(-9);
      }
      for (size_t i = 0; i < within_brtue.size(); i++) {
        if (within_brtue[i].ptr != within_rec[i].ptr) {
          if (within_brtue[i].dist < within_rec[i].dist) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-10);
          }
        }
      }

      std::vector<kd_node_dist<char>> within_it = kd.find_within_iterative(pos, 3.0f);
      if (within_it.size() != within_brtue.size()) {
        fprintf(stderr, "Error at (%f, %f)\n", x, y);
        exit(-11);
      }
      for (size_t i = 0; i < within_brtue.size(); i++) {
        if (within_brtue[i].ptr != within_it[i].ptr) {
          if (within_brtue[i].dist < within_it[i].dist) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-12);
          }
        }
      }
    }
  }

}