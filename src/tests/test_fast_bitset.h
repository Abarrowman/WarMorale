#pragma once

#include "../fast_bitset.h"
#include <catch.hpp>


TEST_CASE("Tests fast_bitset", "[fast_bitset]") {
  SECTION("operator[]") {
    fast_bitset<3> small;
    REQUIRE_FALSE(small[0]);
    REQUIRE_FALSE(small[1]);
    REQUIRE_FALSE(small[2]);

    small[0] = true;
    REQUIRE(small[0]);
    REQUIRE_FALSE(small[1]);
    REQUIRE_FALSE(small[2]);

    small[2] = true;
    REQUIRE(small[0]);
    REQUIRE_FALSE(small[1]);
    REQUIRE(small[2]);

    small[2] = false;
    REQUIRE(small[0]);
    REQUIRE_FALSE(small[1]);
    REQUIRE_FALSE(small[2]);

    fast_bitset<300> big;
    REQUIRE(big.size() == 300);
    bool any_true = false;
    for (size_t i = 0; i < big.size(); i++) {
      any_true |= big[i];
    }
    REQUIRE_FALSE(any_true);

    big[151] = true;
    REQUIRE(big[151]);
    for (size_t i = 0; i < big.size(); i++) {
      if (i != 151) {
        any_true |= big[i];
      }
    }
    REQUIRE_FALSE(any_true);

    big[299] = true;
    big[151] = false;
    REQUIRE(big[299]);

    for (size_t i = 0; i < big.size(); i++) {
      if (i != 299) {
        any_true |= big[i];
      }
    }
    REQUIRE_FALSE(any_true);

    big[297] = true;
    REQUIRE(big[297]);
  }

  SECTION("first_true") {
    fast_bitset<80> medium;

    REQUIRE(medium.first_true() == fast_bitset_utils::npos());


    medium.set(79);
    REQUIRE(medium.first_true() == 79);

    medium.set(60);
    medium.set(58);
    REQUIRE(medium.first_true() == 58);

    medium.reset(58);
    REQUIRE(medium.first_true() == 60);

    medium.set(13);
    REQUIRE(medium.first_true() == 13);

    medium.set(0);
    REQUIRE(medium.first_true() == 0);


    REQUIRE(medium.first_true(1) == 13);
    REQUIRE(medium.first_true(13) == 13);
    REQUIRE(medium.first_true(14) == 60);
    REQUIRE(medium.first_true(61) == 79);

    medium.set(79, false);
    REQUIRE(medium.first_true(61) == fast_bitset_utils::npos());
  
    medium.set(78, true);
    REQUIRE(medium.first_true(61) == 78);

    REQUIRE(medium.first_true(79) == fast_bitset_utils::npos());

    medium.set(60, false);
    REQUIRE(medium.first_true(14) == 78);

    medium.reset(78);
    REQUIRE(medium.first_true(14) == fast_bitset_utils::npos());
  }

  SECTION("first_false") {
    fast_bitset<80> medium;
    medium.flip();

    REQUIRE(medium.first_false() == fast_bitset_utils::npos());


    medium.reset(79);
    REQUIRE(medium.first_false() == 79);

    medium.reset(60);
    medium.reset(58);
    REQUIRE(medium.first_false() == 58);

    medium.set(58);
    REQUIRE(medium.first_false() == 60);

    medium.reset(13);
    REQUIRE(medium.first_false() == 13);

    medium.reset(0);
    REQUIRE(medium.first_false() == 0);


    REQUIRE(medium.first_false(1) == 13);
    REQUIRE(medium.first_false(13) == 13);
    REQUIRE(medium.first_false(14) == 60);
    REQUIRE(medium.first_false(61) == 79);

    medium.flip(79);
    REQUIRE(medium.first_false(61) == fast_bitset_utils::npos());

    medium.set(78, false);
    REQUIRE(medium.first_false(61) == 78);

    REQUIRE(medium.first_false(79) == fast_bitset_utils::npos());

    medium.flip(60);
    REQUIRE(medium.first_false(14) == 78);

    medium.set(78);
    REQUIRE(medium.first_false(14) == fast_bitset_utils::npos());
  }
}