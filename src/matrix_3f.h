#pragma once

#include "utils.h"

#include <cstring>
#include <array>

class matrix_3f {
public:
  std::array<float, 9> values;

  static matrix_3f transformation_matrix(float scale_x = 1.0f, float scale_y = 1.0f, float angle = 0.0f, float trans_x = 0.0f, float trans_y = 0.0f) {
    matrix_3f trans;
    trans.values = {
      scale_x * cos(angle), -scale_y * sin(angle), trans_x,
      scale_x * sin(angle), scale_y * cos(angle), trans_y,
      0, 0, 1
    };
    return trans;
  }

  static matrix_3f orthographic_projection(float left, float right, float bottom, float top) {
    matrix_3f proj;
    proj.values = {
      2 / (right - left), 0, (left + right) / 2,
      0, 2 / (top - bottom), (bottom + top) / 2,
      0, 0, 1
    };
    return proj;
  }

  static matrix_3f identity() {
    matrix_3f mat;
    mat.values = {
      1, 0, 0,
      0, 1, 0,
      0, 0, 1
    };
    return mat;
  }

  float get(int row, int col) const {
    return values[row * 3 + col];
  }

  float& operator()(int row, int col) {
    return values[row * 3 + col];
  }

  void set(int row, int col, float value) {
    values[row * 3 + col] = value;
  }

  matrix_3f operator*(matrix_3f const& c) const {
    matrix_3f result = *this;
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
        float sum = 0;
        for (int idx = 0; idx < 3; idx++) {
          sum += get(row, idx) * c.get(idx, col);
        }
        result.set(row, col, sum);
      }
    }

    return result;
  }

  matrix_3f operator*=(matrix_3f const& c) {
    std::array<float, 9> temp;
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
        float sum = 0;
        for (int idx = 0; idx < 3; idx++) {
          sum += get(row, idx) * c.get(idx, col);
        }
        temp[3 * row + col] = sum;
      }
    }
    values = temp;
    return *this;
  }

};