#pragma once

#include "utils.h"
#include "2d_math.h"
#include <array>
#include <vector>
#include <cstdio>


class vertex_buffer {
public:
  GLuint vbo;
  vertex_buffer() {
    glGenBuffers(1, &vbo);
    bind();
  }

  vertex_buffer(GLuint vertex_buffer_idx) :vbo(vertex_buffer_idx) {}

  // do not copy or assign
  vertex_buffer(vertex_buffer&) = delete;
  vertex_buffer& operator=(const vertex_buffer&) = delete;

  //moving is ok
  vertex_buffer(vertex_buffer&& old) : vbo(old.vbo) {
    old.vbo = 0;
  }

  void bind() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
  }

  ~vertex_buffer() {
    glDeleteBuffers(1, &vbo);
  }
};

class vertex_array {
public:
  GLuint vao;

  vertex_array() {
    glGenVertexArrays(1, &vao);
    bind();
  }

  vertex_array(GLuint vertex_array_idx) :vao(vertex_array_idx) {}

  // do not copy or assign
  vertex_array(vertex_array&) = delete;
  vertex_array& operator=(const vertex_array&) = delete;

  //moving is ok
  vertex_array(vertex_array&& old) : vao(old.vao) {
    old.vao = 0;
  }

  void bind() const {
    glBindVertexArray(vao);
  }

  ~vertex_array() {
    glDeleteVertexArrays(1, &vao);
  }
};


template<size_t U>
inline std::array<vector_2f, U> create_circle_verticies() {
  std::array<vector_2f, U> circle_verts;
  for (int i = 0; i < U; i++) {
    float angle = i * math_consts::tau() / U;
    circle_verts[i] = { cos(angle), sin(angle) };
  }
  return circle_verts;
}

class simple_vertex_array {
private:
  simple_vertex_array(vertex_buffer vertex_buff, vertex_array vertex_arr, GLuint vertex_count) :
    vb(std::move(vertex_buff)), va(std::move(vertex_arr)), size(vertex_count) {
  }

public:
  vertex_buffer vb;
  vertex_array va;
  GLsizei size;

  void set_veritices(float const* vert_ptr, unsigned int vert_count) {
    vb.bind();
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

  static simple_vertex_array create_verticies(float const* vert_ptr, unsigned int vert_count) {
    vertex_buffer vb{};
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 2, vert_ptr, GL_STATIC_DRAW);

    vertex_array va{};
    vb.bind();

    // vetex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    simple_vertex_array array{std::move(vb), std::move(va), vert_count};
    return array;
  }

  static simple_vertex_array create_verticies(std::vector<vector_2f> const& verts) {
    return create_verticies(reinterpret_cast<float const*>(verts.data()), verts.size());
  }

  template<size_t U>
  static simple_vertex_array create_verticies(std::array<vector_2f, U> const& verts) {
    return create_verticies(reinterpret_cast<float const*>(verts.data()), U);
  }

  template<size_t U>
  static simple_vertex_array create_circle() {
    return create_verticies(create_circle_verticies<U>());
  }

  static simple_vertex_array create_sprite_vertex_array() {

    float buffer[] = {
      //   x,    y,    s,    t
      -.5f, -.5f, 0.0f, 1.0f,
      0.5f, -.5f, 1.0f, 1.0f,
      -.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -.5f, 1.0f, 1.0f,
      -.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 1.0f, 0.0f,
    };

    vertex_buffer vb{};
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    vertex_array va{};

    vb.bind();

    // vetex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    // texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));


    simple_vertex_array array{std::move(vb), std::move(va), 6};

    return array;
  }



  // do not copy or assign
  simple_vertex_array(simple_vertex_array&) = delete;
  simple_vertex_array& operator=(const simple_vertex_array&) = delete;

  //moving is ok
  simple_vertex_array(simple_vertex_array&& old) : vb(std::move(old.vb)), va(std::move(old.va)), size(old.size) {
  }

  void draw(GLenum mode) const {
    va.bind();
    glDrawArrays(mode, 0, size);
  }
};