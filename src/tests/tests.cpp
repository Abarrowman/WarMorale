#define CATCH_CONFIG_MAIN 
#include <catch.hpp>

#include "test_kd_tree.h"
#include "test_space_buckets.h"
#include "test_geom.h"
#include "test_sized_vector.h"
#include "test_fast_bitset.h"
#include "test_sparse_container.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? 1 : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
  REQUIRE( Factorial(0) == 1 );
  REQUIRE( Factorial(1) == 1 );
  REQUIRE( Factorial(2) == 2 );
  REQUIRE( Factorial(3) == 6 );
  REQUIRE( Factorial(10) == 3628800 );
}