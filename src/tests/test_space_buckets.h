#pragma once
#include "../space_buckets.h"

inline void test_space_buckets() {
  space_buckets<int> buckets{ 100 };
  buckets.add_entry({ 0, 0 }, 0);
  buckets.add_entry({ 10, 10 }, 1);
  buckets.add_entry({ 50, 110 }, 2);
  buckets.add_entry({ 800, 900 }, 3);

  vector_2f search_loc{ 30.0f, 30.0f };

  auto range = buckets.find_local_bucket(search_loc);
  for (auto it = range.first; it != range.second; ++it) {
    printf("Found Locally %d\n", it->second);
  }

  for (int i : buckets.find_nearby_buckets(search_loc)) {
    printf("Found Nearby %d\n", i);
  }

  buckets.move_entry({ 0, 0 }, { 820, 910 }, 0);

  for (int i : buckets.find_nearby_buckets(search_loc)) {
    printf("Found Nearby After Move %d\n", i);
  }
}