#pragma once

#include "utils.h"
#include "matrix_3f.h"
#include <array>
#include <vector>
#include <cstdio>

class vertex_array {
private:
  vertex_array(GLuint vertex_buffer_idx, GLuint vertex_array_object, GLuint vertex_count) :
    vbo(vertex_buffer_idx), vao(vertex_array_object), size(vertex_count) {
  }

public:
  GLuint vbo;
  GLuint vao;
  GLsizei size;

  void set_veritices(float const* vert_ptr, unsigned int vert_count) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 2, vert_ptr, GL_STATIC_DRAW);
    size = vert_count;
  }

  void set_veritices(std::vector<vector_2f> const& verts) {
    set_veritices(reinterpret_cast<float const*>(verts.data()), verts.size());
  }

  template<size_t U>
  void set_veritices(std::array<vector_2f, U> const& verts) {
    set_veritices(reinterpret_cast<float const*>(verts.data()), U);
  }

  static vertex_array create_verticies(float const* vert_ptr, unsigned int vert_count) {
    GLuint vbo;
    GLuint vao;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 2, vert_ptr, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // vetex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    vertex_array array{vbo, vao, vert_count};
    return array;
  }

  static vertex_array create_verticies(std::vector<vector_2f> const& verts) {
    return create_verticies(reinterpret_cast<float const*>(verts.data()), verts.size());
  }

  template<size_t U>
  static vertex_array create_verticies(std::array<vector_2f, U> const& verts) {
    return create_verticies(reinterpret_cast<float const*>(verts.data()), U);
  }

  static vertex_array create_triangle() {
    std::array<vector_2f, 3> tri_verts{ {
        //   x, y
        { 1, 0 },
        { cos(PI_F * 2 / 3), sin(PI_F * 2 / 3)},
        { cos(PI_F * 2 / 3), sin(PI_F * 4 / 3)}
      } };

    return create_verticies(tri_verts);
  }

  static vertex_array create_sprite_vertex_array() {

    GLuint vbo;
    GLuint vao;

    float buffer[] = {
      //   x,    y,    s,    t
      -.5f, -.5f, 0.0f, 1.0f,
      0.5f, -.5f, 1.0f, 1.0f,
      -.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -.5f, 1.0f, 1.0f,
      -.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 1.0f, 0.0f,
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // vetex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    // texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    vertex_array array{vbo, vao, 6};

    return array;
  }



  // do not copy or assign
  vertex_array(vertex_array&) = delete;
  vertex_array& operator=(const vertex_array&) = delete;

  //moving is ok
  vertex_array(vertex_array&& old) : vbo(old.vbo), vao(old.vao), size(old.size) {
    old.vao = 0;
    old.vbo = 0;
    old.size = 0;
  }

  ~vertex_array() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
  }

  void draw(GLenum mode) const {
    glBindVertexArray(vao);
    glDrawArrays(mode, 0, size);
  }
};