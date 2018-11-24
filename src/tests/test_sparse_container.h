#include "../sparse_container.h"
#include <catch.hpp>


void profile_sparse_container() {
  size_t sum = 0;
  size_t constexpr loop_count = 7000;
  size_t constexpr cap = 1024;
  for (size_t loops = 0; loops < loop_count; loops++) {
    sparse_container<int, cap> con;

    size_t n;
    for (n = 0; n < cap; n++) {
      con.push(n);
    }

    n = 0;
    for(auto it = con.begin(); it != con.end(); ++it, n++) {
      con.erase(it);
      con.push(con.capacity() - n);
    }

    for (n = 0; n < cap; n++) {
      sum += *(con.begin());
      con.erase(con.begin());
    }
  }
  size_t constexpr total_sum = (cap * (cap + 1) / 2) * loop_count;
  REQUIRE(sum == total_sum);
}

TEST_CASE("Tests sparse_container", "[sparse_container]") {
  SECTION("push") {
    sparse_container<int, 3> con;
    REQUIRE(con.size() == 0);
    con.push(1);
    REQUIRE(con.size() == 1);
    con.push(17);
    REQUIRE(con.size() == 2);

    {
      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 2);
      REQUIRE(copy_vector[0] == 1);
      REQUIRE(copy_vector[1] == 17);
    }

    con.push(5);
    REQUIRE(con.size() == 3);

    con.erase(con.begin() + 1);
    REQUIRE(con.size() == 2);
    {
      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 2);
      REQUIRE(copy_vector[0] == 1);
      REQUIRE(copy_vector[1] == 5);
    }

    con.erase(con.begin());
    REQUIRE(con.size() == 1);
    REQUIRE(*(con.begin()) == 5);
    con.push(4);
    REQUIRE(con.size() == 2);

    con.push(3);
    REQUIRE(con.size() == 3);

    {
      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 3);
      REQUIRE(copy_vector[0] == 4);
      REQUIRE(copy_vector[1] == 3);
      REQUIRE(copy_vector[2] == 5);
    }


    con.erase(con.begin() + 1, con.begin() + 3);
    REQUIRE(con.size() == 1);
    REQUIRE(*(con.begin()) == 4);

  }
  
  SECTION("erase") {
    sparse_container<int, 8> con;
    for (size_t i = 0; i < 8; i++) {
      con.push(i);
    }
    REQUIRE(con.size() == 8);

    {
      int idx = 0;
      for (auto it = con.begin(); it != con.end(); ++it, ++idx) {
        REQUIRE(*it == idx);
        if (idx % 2 == 0) {
          con.erase(it);
        }
      }
      REQUIRE(con.size() == 4);

      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 4);
      REQUIRE(copy_vector[0] == 1);
      REQUIRE(copy_vector[1] == 3);
      REQUIRE(copy_vector[2] == 5);
      REQUIRE(copy_vector[3] == 7);
    }
    {
      int idx = 1;
      for (auto it = con.begin(); it != con.end(); ++it, idx += 2) {
        con.push(-idx);
        REQUIRE(*it == idx);
      }
      REQUIRE(con.size() == 8);

      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 8);
      REQUIRE(copy_vector[0] == -1);
      REQUIRE(copy_vector[1] == 1);
      REQUIRE(copy_vector[2] == -3);
      REQUIRE(copy_vector[3] == 3);
      REQUIRE(copy_vector[4] == -5);
      REQUIRE(copy_vector[5] == 5);
      REQUIRE(copy_vector[6] == -7);
      REQUIRE(copy_vector[7] == 7);
    }
    {
      auto one_it = std::find(con.begin(), con.end(), 1);
      REQUIRE(*one_it == 1);

      auto five_it = std::find(con.begin(), con.end(), 5);
      REQUIRE(*five_it == 5);

      con.erase(one_it, ++five_it);

      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 3);
      REQUIRE(copy_vector[0] == -1);
      REQUIRE(copy_vector[1] == -7);
      REQUIRE(copy_vector[2] == 7);
    }
    {
      con.clear();
      REQUIRE(con.size() == 0);
      std::vector<int> copy_vector;
      std::copy(con.begin(), con.end(), std::back_inserter(copy_vector));
      REQUIRE(copy_vector.size() == 0);
    }
  }

  /*SECTION("profile") {
    profile_sparse_container();
  }*/
}