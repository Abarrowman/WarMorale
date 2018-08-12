#pragma once
#include "../space_buckets.h"
#include <catch.hpp>
#include <iterator>
#include <algorithm>

TEST_CASE("space_buckets can be built and searched", "[space_buckets]") {
  space_buckets<int> buckets{ 100 };
  buckets.add_entry({ 0, 0 }, 0);
  buckets.add_entry({ 10, 10 }, 1);
  buckets.add_entry({ 50, 110 }, 2);
  buckets.add_entry({ 800, 900 }, 3);

  vector_2f search_loc{ 30.0f, 30.0f };
  vector_2f second_loc{ 800.0f, 900.0f };

  SECTION("find_local_bucket finds the values in the local bucket") {
    std::vector<int>* bucket = buckets.find_local_bucket(search_loc);
    REQUIRE(bucket != nullptr);
    std::vector<int>& bucket_ref = *bucket;
    REQUIRE(bucket_ref.size() == 2);
    REQUIRE(bucket_ref[0] == 0);
    REQUIRE(bucket_ref[1] == 1);

    std::vector<int>* before_bucket = buckets.find_local_bucket(second_loc);
    REQUIRE(before_bucket != nullptr);
    std::vector<int>& before_ref = *before_bucket;
    REQUIRE(before_ref.size() == 1);
    REQUIRE(before_ref[0] == 3);
  }

  SECTION("find_nearby_buckets finds the values in the nearby buckets") {
    sized_vector<std::vector<int>*, 9> found = buckets.find_nearby_buckets(search_loc);
    REQUIRE(std::distance(found.begin(), found.end()) == 2);

    std::vector<int> found_vec;
    for (auto&& vec_ptr : found) {
      for (int val : *vec_ptr) {
        found_vec.push_back(val);
      }
    }

    REQUIRE(found_vec.size() == 3);
    REQUIRE(found_vec[0] == 0);
    REQUIRE(found_vec[1] == 1);
    REQUIRE(found_vec[2] == 2);
  }
  SECTION("move_entry properly moves an entry") {
    buckets.move_entry({ 0, 0 }, { 820, 910 }, 0);

    std::vector<int>* after_bucket = buckets.find_local_bucket(second_loc);
    REQUIRE(after_bucket != nullptr);
    std::vector<int>& after_ref = *after_bucket;
    REQUIRE(after_ref.size() == 2);
    REQUIRE(after_ref[0] == 3);
    REQUIRE(after_ref[1] == 0);

    sized_vector<std::vector<int>*, 9> found = buckets.find_nearby_buckets(search_loc);
    REQUIRE(std::distance(found.begin(), found.end()) == 2);

    std::vector<int> found_vec;
    for (auto&& vec_ptr : found) {
      for (int val : *vec_ptr) {
        found_vec.push_back(val);
      }
    }
    REQUIRE(found_vec.size() == 2);
    REQUIRE(found_vec[0] == 1);
    REQUIRE(found_vec[1] == 2);
  }
}