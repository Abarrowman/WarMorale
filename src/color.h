#pragma once

#include <array>

class color {
public:
  std::array<float, 4> floats = { 1, 1, 1, 1 };

  static color red(float alpha = 1.0f) {
    return color({1.0f, 0.0f, 0.0f, alpha});
  }

  static color green(float alpha = 1.0f) {
    return color({ 0.0f, 1.0f, 0.0f, alpha });
  }

  static color blue(float alpha = 1.0f) {
    return color({ 0.0f, 0.0f, 1.0f, alpha });
  }

  static color white(float alpha = 1.0f) {
    return color({ 1.0f, 1.0f, 1.0f, alpha });
  }

  static color black (float alpha = 1.0f) {
    return color({ 0.0f, 0.0f, 0.0f, alpha });
  }
};
