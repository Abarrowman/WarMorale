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

#define PI_F 3.14159265358979323846f

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

inline std::string read_file_to_string(std::string const& filename) {
  std::ifstream inFile(filename, std::ios::in | std::ios::binary);
  if (inFile) {
    std::ostringstream strStream;
    strStream << inFile.rdbuf();
    inFile.close();
    return strStream.str();
  }
  fprintf(stderr, "Could not read file %s\n", filename.c_str());
  exit(-1);
}

template<typename T>
inline void push_unique(std::vector<T>& vec, T const& val) {
  if (std::find(vec.begin(), vec.end(), val) == vec.end()) {
    vec.push_back(val);
  }
}

inline float angle_err(float const current_ang, float const target_ang) {
  float ang_err = target_ang - current_ang;
  if (ang_err > PI_F) {
    ang_err = -2 * PI_F + ang_err;
  } else if (ang_err < -PI_F) {
    ang_err = 2 * PI_F + ang_err;
  }
  return ang_err;
}

inline float angle_clamp(float const angle) {
  if (angle > PI_F) {
    return -2 * PI_F + angle;
  } else if (angle < -PI_F) {
    return 2 * PI_F + angle;
  } else {
    return angle;
  }
}