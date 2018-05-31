#pragma once

#include <cmath>
#include <cstring>
#include <array>

class vector_2f;

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

  vector_2f operator*(vector_2f const& c) const;

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

class vector_2f {
public:
  float x;
  float y;

  vector_2f() {};
  vector_2f(float vx, float vy) : x(vx), y(vy)  {};


  float magnitude() const {
    return std::sqrt(x * x + y * y);
  }

  float angle() const {
    return std::atan2(y, x);
  }

  vector_2f operator-(vector_2f const& other) const {
    return { x - other.x, y - other.y };
  }

  vector_2f operator+(vector_2f const& other) const {
    return { x + other.x, y + other.y };
  }

  vector_2f operator+=(vector_2f const& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  vector_2f operator*(float other) const {
    return { x * other, y * other };
  }

  vector_2f operator*=(float other) {
    x *= other;
    y *= other;
    return *this;
  }

  static vector_2f create_polar(float ang) {
    return { cos(ang), sin(ang) };
  }

  static vector_2f create_polar(float ang, float mag) {
    return create_polar(ang) * mag;
  }

};

class trans_state {
public:
  float scale_x = 1.0f;
  float scale_y = 1.0f;
  float angle = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  
  matrix_3f to_matrix() {
    return matrix_3f::transformation_matrix(scale_x, scale_y, angle, x, y);
  }

  vector_2f translation_to(trans_state const& other) const {
    return other.get_position() - get_position();
  }

  vector_2f translation_to(vector_2f const& other) const {
    return other - get_position();
  }

  vector_2f get_position() const {
    return {x, y};
  }

  void set_position(vector_2f pos) {
    x = pos.x;
    y = pos.y;
  }

  void set_clamped_angle(float unclamped_angle) {
    angle = angle_clamp(unclamped_angle);
  }

  void clamp_angle() {
    angle = angle_clamp(angle);
  }
};

inline vector_2f matrix_3f::operator*(vector_2f const& c) const {
  return { values[0] * c.x + values[1] * c.y + values[2], values[3] * c.x + values[4] * c.y + values[5] };
}
