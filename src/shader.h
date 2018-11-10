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

  GLuint shader_idx_;
  GLenum type_;

public:
  // do not copy, assign, or move assign
  shader(shader&) = delete;
  shader& operator=(const shader&) = delete;
  shader& operator= (shader&& old) = delete;

  //moving is ok
  shader(shader&& old) : shader_idx_(old.shader_idx_), type_(old.type_) {
    old.shader_idx_ = 0;
  }

  shader(int type, const char* shader_src) {
    shader_idx_ = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(shader_idx_, 1, &shader_src, NULL);
    compile_and_check(shader_idx_);
  }

  ~shader() {
    glDeleteShader(shader_idx_);
    shader_idx_ = 0;
  }

  GLuint shader_idx() const {
    return shader_idx_;
  }

  GLenum type() const {
    return type_;
  }
};

class program {
private:
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

  template<typename... Args>
  void attach_shaders(shader& shader, Args&... programs) {
    glAttachShader(prog, shader.shader_idx());
    attach_shaders(programs...);
  }

  void attach_shaders(shader& shader) {
    glAttachShader(prog, shader.shader_idx());
  }


  template<typename... Args>
  void detach_shaders(shader& shader, Args&... programs) {
    glDetachShader(prog, shader.shader_idx());
    detach_shaders(programs...);
  }

  void detach_shaders(shader& shader) {
    glAttachShader(prog, shader.shader_idx());
  }

  GLuint prog;

public:

  template<typename... Args>
  program(Args&... programs) {
    prog = glCreateProgram();
    attach_shaders(programs...);  
    link_and_check(prog);
    detach_shaders(programs...);
  }

  GLint get_uniform_location(char const* name) {
    return glGetUniformLocation(prog, name);
  }

  // do not copy or assign
  program(program&) = delete;
  program& operator=(const program&) = delete;

  //moving is ok
  program(program&& old) : prog(old.prog) {
    old.prog = 0;
  }
  // move assinging is not ok
  program& operator= (program&& old) = delete;

  ~program() {
    glDeleteProgram(prog);
    prog = 0;
  }

  void use() const {
    glUseProgram(prog);
  }
};