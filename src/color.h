#pragma once

#include <array>

class color {
public:
  std::array<float, 4> floats = { 1, 1, 1, 1 };

  color with_alpha(float alpha) const {
    return color({ floats[0], floats[1], floats[2], alpha });
  }

  static color red() {
    return color({1.0f, 0.0f, 0.0f, 1.0f});
  }

  static color green() {
    return color({ 0.0f, 1.0f, 0.0f, 1.0f });
  }

  static color blue() {
    return color({ 0.0f, 0.0f, 1.0f, 1.0f });
  }

  static color white() {
    return color({ 1.0f, 1.0f, 1.0f, 1.0f });
  }

  static color black () {
    return color({ 0.0f, 0.0f, 0.0f, 1.0f });
  }

  static color transparent_black() {
    return color({ 0.0f, 0.0f, 0.0f, 0.0f });
  }
};
