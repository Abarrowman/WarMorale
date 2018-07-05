#pragma once

#include <array>

class color_rgba {
public:
  std::array<float, 4> values = { 1, 1, 1, 1 };

  color_rgba with_alpha(float alpha) const {
    return color_rgba({ values[0], values[1], values[2], alpha });
  }

  static color_rgba red() {
    return color_rgba({1.0f, 0.0f, 0.0f, 1.0f});
  }

  static color_rgba green() {
    return color_rgba({ 0.0f, 1.0f, 0.0f, 1.0f });
  }

  static color_rgba blue() {
    return color_rgba({ 0.0f, 0.0f, 1.0f, 1.0f });
  }

  static color_rgba white() {
    return color_rgba({ 1.0f, 1.0f, 1.0f, 1.0f });
  }

  static color_rgba black () {
    return color_rgba({ 0.0f, 0.0f, 0.0f, 1.0f });
  }

  static color_rgba transparent_black() {
    return color_rgba({ 0.0f, 0.0f, 0.0f, 0.0f });
  }

  // TODO consider creating vector_4<T>
  color_rgba operator*=(color_rgba const& other) {
    values[0] *= other.values[0];
    values[1] *= other.values[1];
    values[2] *= other.values[2];
    values[3] *= other.values[3];

    return *this;
  }

  color_rgba operator*(color_rgba const& other) const {
    return { values[0] * other.values[0], values[1] * other.values[1], values[2] * other.values[2], values[3] * other.values[3] };
  }
};

class color_rgb {
public:
  std::array<float, 3> values = { 1, 1, 1};


  static color_rgb red() {
    return color_rgb({ 1.0f, 0.0f, 0.0f });
  }

  static color_rgb green() {
    return color_rgb({ 0.0f, 1.0f, 0.0f });
  }

  static color_rgb blue() {
    return color_rgb({ 0.0f, 0.0f, 1.0f});
  }

  static color_rgb white() {
    return color_rgb({ 1.0f, 1.0f, 1.0f });
  }

  static color_rgb black() {
    return color_rgb({ 0.0f, 0.0f, 0.0f });
  }



  color_rgba with_alpha(float alpha = 1.0f) const {
    return color_rgba({ values[0], values[1], values[2], alpha });
  }
};