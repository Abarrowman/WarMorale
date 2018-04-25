#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include "utils.h"

class vertex_array {
public:
  GLuint vbo;
  GLuint vao;

  vertex_array() {
    float buffer[] = {
    //   x,    y,    z,    s,    t
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(float), buffer, GL_STATIC_DRAW);
    //check_gl_errors();
    glGenVertexArrays(1, &vao);
    //check_gl_errors();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // vetex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    // texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

  }

  // do not copy or move
  vertex_array(vertex_array&) = delete;
  vertex_array(vertex_array&&) = delete;
  vertex_array& operator=(const vertex_array&) = delete;

  virtual ~vertex_array() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
  }
};