#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include "utils.h"

class object {
public:
  GLuint vbo;
  GLuint vao;

  object() {
    float points[] = {
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
    //check_gl_errors();
    glGenVertexArrays(1, &vao);
    //check_gl_errors();
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  // do not copy or move
  object(object&) = delete;
  object(object&&) = delete;
  object& operator=(const object&) = delete;

  virtual ~object() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
  }
};