#pragma once
#include "../sized_vector.h"
#include <catch.hpp>
#include <cmath>

TEST_CASE("Tests sized_vector", "[sized_vector]") {
  sized_vector<int, 10> s;
  REQUIRE(s.capacity() == 10);
  s.emplace_back(1);
  s.emplace_back(2);
  s.emplace_back(4);

  SECTION("basic") {
    REQUIRE(s.size() == 3);
    REQUIRE(s[0] == 1);
    REQUIRE(s[1] == 2);
    REQUIRE(s[2] == 4);
  }

  SECTION("front and back") {
    REQUIRE(s.front() == 1);
    s.front() = 6;
    REQUIRE(s.front() == 6);

    REQUIRE(s.back() == 4);
    s.back() = 5;
    REQUIRE(s.back() == 5);

    s.emplace_back(8);
    REQUIRE(s.back() == 8);
  }

  SECTION("push_back") {
    s.push_back(8);
    REQUIRE(s.size() == 4);
    REQUIRE(s[3] == 8);
  }

  SECTION("pop_back") {
    s.pop_back();
    REQUIRE(s.size() == 2);
  }
  SECTION("single element erase") {
    SECTION("erase first element") {
      int* it = s.erase(s.cbegin());
      REQUIRE(it == s.begin());
      REQUIRE((*it) == 2);
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 2);
      REQUIRE(s[1] == 4);
    }
    SECTION("erase middle element") {
      int* it = s.erase(s.cbegin() + 1);
      REQUIRE(it == (s.begin() + 1));
      REQUIRE((*it) == 4);
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 1);
      REQUIRE(s[1] == 4);
    }
    SECTION("erase last element") {
      int* it = s.erase(s.cbegin() + 2);
      REQUIRE(it == s.end());
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 1);
      REQUIRE(s[1] == 2);
    }
  }
  SECTION("multiple element erase") {
    s.push_back(8);
    // [1, 2, 4, 8]

    SECTION("erase first elements") {
      int* it = s.erase(s.cbegin(), s.cbegin() + 2);
      REQUIRE(it == s.begin());
      REQUIRE((*it) == 4);
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 4);
      REQUIRE(s[1] == 8);
    }
    SECTION("erase middle elements") {
      int* it = s.erase(s.cbegin() + 1, s.cbegin() + 3);
      REQUIRE(it == (s.begin() + 1));
      REQUIRE((*it) == 8);
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 1);
      REQUIRE(s[1] == 8);
    }
    SECTION("erase last elements") {
      int* it = s.erase(s.cbegin() + 2, s.cend());
      REQUIRE(it == s.end());
      REQUIRE(s.size() == 2);
      REQUIRE(s[0] == 1);
      REQUIRE(s[1] == 2);
    }
  }
  SECTION("empty and clear") {
    REQUIRE_FALSE(s.empty());

    s.clear();
    REQUIRE(s.capacity() == 10);
    REQUIRE(s.size() == 0);
    REQUIRE(s.empty());
  }
  SECTION("data") {
    int* d = s.data();
    REQUIRE(d[0] == 1);
    REQUIRE(d[1] == 2);
    REQUIRE(d[2] == 4);

    s[1] = 7;
    REQUIRE(d[1] == 7);

    d[2] = 8;
    REQUIRE(s[2] == 8);
  }
  SECTION("reisze") {
    s.resize(0);
    REQUIRE(s.size() == 0);

    s.resize(4);
    REQUIRE(s.size() == 4);

    s.resize(5, 6);
    REQUIRE(s.size() == 5);
    REQUIRE(s[4] == 6);
  }
  SECTION("assignment copy constructor") {
    sized_vector<int, 10> other = s;
    REQUIRE(other.size() == 3);
    REQUIRE(other[0] == 1);
    REQUIRE(other[1] == 2);
    REQUIRE(other[2] == 4);
  }

  SECTION("assignment move constructor") {
    sized_vector<int, 10> other = std::move(s);
    REQUIRE(other.size() == 3);
    REQUIRE(other[0] == 1);
    REQUIRE(other[1] == 2);
    REQUIRE(other[2] == 4);
  }

  SECTION("copy constructor") {
    sized_vector<int, 10> other{ s };
    REQUIRE(other.size() == 3);
    REQUIRE(other[0] == 1);
    REQUIRE(other[1] == 2);
    REQUIRE(other[2] == 4);
  }

  SECTION("move constructor") {
    sized_vector<int, 10> other{ std::move(s) };
    REQUIRE(other.size() == 3);
    REQUIRE(other[0] == 1);
    REQUIRE(other[1] == 2);
    REQUIRE(other[2] == 4);
  }

  SECTION("swap") {
    sized_vector<int, 10> other;
    other.emplace_back(6);
    other.emplace_back(7);
    other.emplace_back(8);
    other.emplace_back(9);
    s.swap(other);
    REQUIRE(other.size() == 3);
    REQUIRE(other[0] == 1);
    REQUIRE(other[1] == 2);
    REQUIRE(other[2] == 4);
    REQUIRE(s.size() == 4);
    REQUIRE(s[0] == 6);
    REQUIRE(s[1] == 7);
    REQUIRE(s[2] == 8);
    REQUIRE(s[3] == 9);
  }
}