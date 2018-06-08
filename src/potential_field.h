#pragma once

#include "2d_math.h"

inline float quadratic_height(vector_2f center, vector_2f pos) {
  return (pos - center).magnitude_squared();
}

inline vector_2f quadratic_gradient(vector_2f center, vector_2f pos) {
  return 2.0f * (pos - center);
}

inline float cone_height(vector_2f center, vector_2f pos) {
  return (pos - center).magnitude();
}

inline vector_2f cone_gradient(vector_2f center, vector_2f pos) {
  vector_2f diff = pos - center;
  float mag = diff.magnitude();
  if (mag == 0) {
    return vector_2f::zero();
  } else {
    return (1 / mag) * diff;
  }
}

inline float quadratic_cone_height(vector_2f center, vector_2f pos, float change_distance) {
  vector_2f diff = pos - center;
  float distance_squared = diff.magnitude_squared();
  float distance = std::sqrt(distance_squared);
  if (distance <= change_distance) {
    return distance_squared;
  } else {
    return change_distance * (2 * distance - change_distance);
  }
}

inline vector_2f quadratic_cone_gradient(vector_2f center, vector_2f pos, float change_distance) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance <= change_distance) {
    return diff * 2;
  } else {
    return (2 * change_distance / distance) * diff;
  }
}

inline float signularity_height(vector_2f center, vector_2f pos, float radius) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance > radius) {
    return 0;
  } else {
    float val = (1 / distance - 1 / radius);
    return val * val;
  }
}

inline vector_2f signularity_gradient(vector_2f center, vector_2f pos, float radius) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance == 0) {
    return vector_2f::zero();
  } else if (distance > radius) {
    return vector_2f::zero();
  } else {
    vector_2f result = (2.0f * ((1.0f / radius) - (1.0f / distance)) / (distance * distance * distance)) * diff;
    return result;
  }
}

inline float downwards_cone_height(vector_2f center, vector_2f pos, float max_radius) {
  // TODO negative heights
  return max_radius - (pos - center).magnitude();
}

inline vector_2f downwards_cone_gradient(vector_2f center, vector_2f pos, float max_radius) {
  vector_2f diff = pos - center;
  float mag = diff.magnitude();
  if ((mag == 0) || (mag > max_radius)) {
    return vector_2f::zero();
  } else {
    return (-1 / mag) * diff;
  }
}
inline float downwards_quadratic_height(vector_2f center, vector_2f pos, float max_radius) {
  float val = max_radius - (pos - center).magnitude();
  return val * val;
}

inline vector_2f downwards_quadratic_gradient(vector_2f center, vector_2f pos, float max_radius) {
  vector_2f diff = pos - center;
  float mag = diff.magnitude();
  if ((mag == 0) || (mag > max_radius)) {
    return vector_2f::zero();
  } else {
    return (2.0f * (max_radius * (1.0f / mag) - 1.0f)) * diff;
  }
}