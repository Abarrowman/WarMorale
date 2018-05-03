#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class shader {
public:
  GLuint vs;
  GLuint gs;
  GLuint fs;
  GLuint program;

  shader(const char* vertex_shader, const char* geometry_shader, const char* fragment_shader) {

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    gs = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gs, 1, &geometry_shader, NULL);
    glCompileShader(gs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glAttachShader(program, gs);
    glLinkProgram(program);

    check_gl_errors();

  }

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

    gs = 0;
  }

  // do not copy or assign
  shader(shader&) = delete;
  shader& operator=(const shader&) = delete;

  //moving is ok
  shader(shader&& old) : program(old.program), vs(old.vs), gs(old.gs), fs(old.fs) {
    old.program = 0;
    old.vs = 0;
    old.gs = 0;
    old.fs = 0;
  }

  ~shader() {
    glDeleteProgram(program);
    glDeleteShader(fs);
    glDeleteShader(gs);
    glDeleteShader(vs);
  }

  void use() const {
    glUseProgram(program);
  }
};