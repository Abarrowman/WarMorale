#pragma once

#include "2d_math.h"

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
    return -2.0f * diff;
  } else {
    return (-2.0f * change_distance / distance) * diff;
  }
}

inline float obstacle_height(vector_2f center, vector_2f pos, float radius) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance > radius) {
    return 0;
  } else {
    float val = (1 / distance - 1 / radius);
    return val * val;
  }
}

inline vector_2f fractional_obstacle_gradient(vector_2f center, vector_2f pos, float radius) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance == 0) {
    return vector_2f::zero(); // on top of each other
  } else if (distance > radius) {
    return vector_2f::zero();
  }

  vector_2f distance_grad = (1.0f / distance) * diff;
  float grad_scale = (-2.0f * (1.0f / radius - 1.0f / distance) / (distance * distance));
  return grad_scale * distance_grad;
}

inline vector_2f normalized_fractional_obstacle_gradient(vector_2f center, vector_2f pos, float radius) {
  return radius * radius * radius * fractional_obstacle_gradient(center, pos, radius);
}

inline vector_2f absolute_obstacle_gradient(vector_2f center, vector_2f pos, float radius, float min_radius) {
  vector_2f diff = pos - center;
  float distance = diff.magnitude();
  if (distance == 0) {
    return vector_2f::zero(); // on top of each other
  } else if (distance > radius) {
    return vector_2f::zero();
  }
  if (distance < min_radius ) {
    return vector_2f::zero();
  }

  float delta = radius - distance;
  vector_2f distance_grad = (1.0f / distance) * diff;
  float grad_scale = (-2.0f * (1.0f / min_radius - 1.0f / delta) / (delta * delta));
  return grad_scale * distance_grad;
}

inline vector_2f normalized_absolute_obstacle_gradient(vector_2f center, vector_2f pos, float radius, float min_radius) {
  return min_radius * min_radius * min_radius * absolute_obstacle_gradient(center, pos, radius, min_radius);
}

inline float gaussian_height(vector_2f center, vector_2f pos, float std_dev) {
  vector_2f diff = pos - center;
  vector_2f diff_sq = diff.element_squared();
  float variance = std_dev * std_dev;
  return (exp(-diff_sq.x / (4 * variance)) * exp(-diff_sq.y / (4 * variance))) / (2 * math_consts::pi() * variance);
}

inline vector_2f gaussian_gradient(vector_2f center, vector_2f pos, float std_dev) {
  vector_2f diff = pos - center;
  vector_2f diff_sq = diff.element_squared();
  float variance = std_dev * std_dev;
  float common = (exp(-diff_sq.x / (4 * variance))*exp(-diff_sq.y / (4 * variance))) / (4 * variance * variance * math_consts::pi());
  return diff * common;
}

inline vector_2f normalized_gaussian_gradient(vector_2f center, vector_2f pos, float std_dev) {
  float variance = std_dev * std_dev;
  return math_consts::pi() * math_consts::e() * variance * variance * gaussian_gradient(center, pos, std_dev);
}


inline vector_2f absolute_gaussian_gradient(vector_2f center, vector_2f pos, float std_dev, float min_radius) {
  vector_2f raw_diff = pos - center;
  float distance = raw_diff.magnitude();

  if (distance == 0) {
    return vector_2f::zero();
  }

  vector_2f min_center = center + (min_radius / distance) * raw_diff;

  vector_2f diff = pos - min_center;
  vector_2f diff_sq = diff.element_squared();

  float variance = std_dev * std_dev;
  float common = (exp(-diff_sq.x / (4 * variance))*exp(-diff_sq.y / (4 * variance))) / (4 * variance * variance * math_consts::pi());

  if (distance < min_radius + 20) {
    common = common;
  }

  if (distance < min_radius) {
    common *= -1.0f;
  }

  return diff * common;
}

inline vector_2f normalized_absolute_gaussian_gradient(vector_2f center, vector_2f pos, float std_dev, float min_radius) {
  float variance = std_dev * std_dev;
  return math_consts::pi() * math_consts::e() * variance * variance * absolute_gaussian_gradient(center, pos, std_dev, min_radius);
}

/*inline vector_2f gaussian_gradient(vector_2f center, vector_2f pos, vector_2f direction, float std_dev) {
  vector_2f diff = pos - center;
  vector_2f diff_sq = diff.element_squared();
  float variance = std_dev * std_dev;
  float common = (exp(-diff_sq.x / (4 * variance))*exp(-diff_sq.y / (4 * variance))) / (4 * variance * variance * math_consts::pi());
  vector_2f unprojected = diff * common;
}*/