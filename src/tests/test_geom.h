#pragma once
#include "../geom.h"
#include <catch.hpp>
#include <cmath>

TEST_CASE("point_to_segment_distance correctly measures distances", "[geom]") {
  
  SECTION("horizontal lines") {
    SECTION("points on line") {
      REQUIRE(point_to_segment_distance({ 0.0f, 0.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 50.0f, 0.0f }, { { 40.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 100.0f, 0.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(0.0f));
    }
    SECTION("points along segment") {
      REQUIRE(point_to_segment_distance({ 0.0f, 5.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(5.0f));
      REQUIRE(point_to_segment_distance({ -50.0f, -5.0f }, { { 0.0f, 0.0f }, { -100.0f, 0.0f } }) == Approx(5.0f));
      REQUIRE(point_to_segment_distance({ 100.0f, 10.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(10.0f));
    }
    SECTION("points beyond segment") {
      REQUIRE(point_to_segment_distance({ -1.0f, 0.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(1.0f));
      REQUIRE(point_to_segment_distance({ -1.0f, -1.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(std::sqrt(2.0f)));

      REQUIRE(point_to_segment_distance({ 101.0f, 0.0f }, { { 0.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(1.0f));
      REQUIRE(point_to_segment_distance({ 101.0f, 3.0f }, { { 50.0f, 0.0f }, { 100.0f, 0.0f } }) == Approx(std::sqrt(10.0f)));
    }
  }

  SECTION("vertical lines") {
    SECTION("points on line") {
      REQUIRE(point_to_segment_distance({ 0.0f, 0.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 0.0f, 50.0f }, { { 0.0f, 30.0f }, { 0.0f, 100.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 0.0f, 100.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(0.0f));
    }
    SECTION("points along segment") {
      REQUIRE(point_to_segment_distance({ 5.0f, 0.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(5.0f));
      REQUIRE(point_to_segment_distance({ -5.0f, 50.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(5.0f));
      REQUIRE(point_to_segment_distance({ 10.0f, -100.0f }, { { 0.0f, 0.0f }, { 0.0f, -100.0f } }) == Approx(10.0f));
    }
    SECTION("points beyond segment") {
      REQUIRE(point_to_segment_distance({ 0.0f, -3.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(3.0f));
      REQUIRE(point_to_segment_distance({ -1.0f, -1.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(std::sqrt(2.0f)));

      REQUIRE(point_to_segment_distance({ 0.0f, 101.0f }, { { 0.0f, 0.0f }, { 0.0f, 100.0f } }) == Approx(1.0f));
      REQUIRE(point_to_segment_distance({ 2.0f, 101.0f }, { { 0.0f, 80.0f }, { 0.0f, 100.0f } }) == Approx(std::sqrt(5.0f)));
    }
  }

  SECTION("sloped lines") {
    SECTION("points on line") {
      REQUIRE(point_to_segment_distance({ 2.0f, 2.0f }, { { 1.0f, 1.0f }, { 4.0f, 4.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 0.0f, 0.0f }, { { -4.0f, 1.0f }, { 4.0f, -1.0f } }) == Approx(0.0f));
      REQUIRE(point_to_segment_distance({ 100.0f, 50.0f }, { { 0.0f, 0.0f }, { 100.0f, 50.0f } }) == Approx(0.0f));
    }
    SECTION("points along segment") {
      REQUIRE(point_to_segment_distance({ 4.0f, 6.0f }, { { 0.0f, 0.0f }, { 10.0f, 10.0f } }) == Approx(std::sqrt(2.0f)));
      REQUIRE(point_to_segment_distance({ 9.0f, 7.0f }, { { 5.0f, 2.5f }, { 20.0f, 10.0f } }) == Approx(std::sqrt(5.0f)));
    }
    SECTION("points beyond segment") {
      REQUIRE(point_to_segment_distance({ 24.0f, 12.0f }, { { 5.0f, 2.5f }, { 20.0f, 10.0f } }) == Approx(std::sqrt(20.0f)));
      REQUIRE(point_to_segment_distance({ 23.0f, 14.0f }, { { 5.0f, 2.5f }, { 20.0f, 10.0f } }) == Approx(5.0f));
    }
  }
}