#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include <string>
#include <fstream>
#include <sstream>

inline void check_gl_errors() {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "Error %#010x ", err);
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