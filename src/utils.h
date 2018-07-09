#pragma once
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <random>
#include <limits>

using generator_type = std::minstd_rand;

float rand_float(generator_type& gen) {
  return std::uniform_real_distribution<float>{}(gen);
}

float rand_centered_float(generator_type& gen) {
  return std::uniform_real_distribution<float>{-1.0, 1.0f}(gen);
}

bool rand_bool(generator_type& gen) {
  return (rand_float(gen) > 0.5f);
}

inline void check_gl_errors() {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "Polled Error %#010x ", err);
    switch (err) {
    case GL_INVALID_ENUM:
      fprintf(stderr, "GL_INVALID_ENUM\n");
      break;
    case GL_INVALID_OPERATION:
      fprintf(stderr, "GL_INVALID_OPERATION\n");
      break;
    default:
      fprintf(stderr, "UNKNOWN\n");
    }
  }
}

template<typename ... Args>
std::string string_format(std::string const& format, Args ... args) {
  size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // leave space for 0 suffix
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), args ...);
  return std::string(buf.get(), buf.get() + size - 1); // remove 0 suffix
}

inline std::string read_file_to_string(char const* filename) {
  std::ifstream in_file(filename, std::ios::in | std::ios::binary);
  if (in_file) {
    std::ostringstream strStream;
    strStream << in_file.rdbuf();
    in_file.close();
    return strStream.str();
  }
  fprintf(stderr, "Could not read file %s\n", filename);
  exit(-1);
}

template<typename T>
inline void push_unique(std::vector<T>& vec, T const& val) {
  if (std::find(vec.begin(), vec.end(), val) == vec.end()) {
    vec.push_back(val);
  }
}

template<typename T>
inline bool equal_with_tolerance(T left, T right, T tolerance) {
  return (std::abs(left - right) < tolerance);
}

inline bool float_equal(float left, float right, float tolerance = 0.00001) {
  return equal_with_tolerance(left, right, tolerance);
}

template<typename T>
inline float value_clamp(T min, T max, T value) {
  return std::min(max, std::max(value, min));
}

template<typename T>
inline float absolute_value_clamp(T abs_max, T value) {
  if (value > 0) {
    return std::min(abs_max, value);
  } else {
    return std::max(-abs_max, value);
  }
}