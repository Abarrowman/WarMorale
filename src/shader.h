#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "gl_includes.h"

class shader {
private:
  static void compile_and_check(GLuint shader_idx) {
    glCompileShader(shader_idx);
    GLint success = 0;
    glGetShaderiv(shader_idx, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLint log_buffer_size = 0;
      glGetShaderiv(shader_idx, GL_INFO_LOG_LENGTH, &log_buffer_size);
      std::vector<GLchar> error_log(log_buffer_size);
      GLint log_length = 0;
      glGetShaderInfoLog(shader_idx, log_buffer_size, &log_length, error_log.data());
      fprintf(stderr, "Shader Compile Error [%.*s]\n", log_length, error_log.data());
    }
  }

  static void link_and_check(GLuint program_idx) {
    glLinkProgram(program_idx);
    GLint success = 0;
    glGetProgramiv(program_idx, GL_LINK_STATUS, &success);
    if (!success) {
      GLint log_buffer_size = 0;
      glGetProgramiv(program_idx, GL_INFO_LOG_LENGTH, &log_buffer_size);
      std::vector<GLchar> error_log(log_buffer_size);
      GLint log_length = 0;
      glGetProgramInfoLog(program_idx, log_buffer_size, &log_length, error_log.data());
      fprintf(stderr, "Program Link Error [%.*s]\n", log_length, error_log.data());
    }
  }

public:
  GLuint vs;
  GLuint gs;
  GLuint fs;
  GLuint program;

  shader(const char* vertex_shader, const char* geometry_shader, const char* fragment_shader) {

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    compile_and_check(vs);

    gs = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gs, 1, &geometry_shader, NULL);
    compile_and_check(gs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    compile_and_check(fs);

    program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glAttachShader(program, gs);
    link_and_check(program);
  }

  shader(const char* vertex_shader, const char* fragment_shader) {
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    compile_and_check(vs);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    compile_and_check(fs);

    program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    link_and_check(program);

    gs = 0;
  }

  GLint get_uniform_location(char const* name) {
    return glGetUniformLocation(program, name);
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
  // move assinging is not ok
  shader& operator= (shader&& old) = delete;

  ~shader() {
    glDeleteProgram(program);
    glDeleteShader(fs);
    glDeleteShader(gs);
    glDeleteShader(vs);
    program = 0;
    fs = 0;
    gs = 0;
    vs = 0;
  }

  void use() const {
    glUseProgram(program);
  }
};