#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class shader {
public:
  GLuint vs;
  GLuint fs;
  GLuint program;

  shader(const char* vertex_shader, const char* fragment_shader) {
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glLinkProgram(program);

    check_gl_errors();
  }

  // do not copy or move
  shader(shader&) = delete;
  shader(shader&&) = delete;
  shader& operator=(const shader&) = delete;

  ~shader() {
    glDeleteProgram(program);
    glDeleteShader(fs);
    glDeleteShader(vs);
  }
};