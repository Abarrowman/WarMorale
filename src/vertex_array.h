#pragma once

#include "gl_includes.h"
#include "2d_math.h"
#include <array>
#include <vector>
#include <cstdio>

struct textured_vertex {
  vector_2f vert;
  vector_2f tex;

  textured_vertex(vector_2f v, vector_2f t) : vert(std::move(v)), tex(std::move(t)) {}
};

class vertex_buffer {
public:
  GLuint vbo;
  vertex_buffer() {
    glGenBuffers(1, &vbo);
    bind();
  }

  vertex_buffer(GLuint vertex_buffer_idx) :vbo(vertex_buffer_idx) {}

  // do not copy or copy assign
  vertex_buffer(vertex_buffer&) = delete;
  vertex_buffer& operator=(const vertex_buffer&) = delete;

  // moving is ok
  vertex_buffer(vertex_buffer&& old) : vbo(old.vbo) {
    old.vbo = 0;
  }
  // move assigning is not ok
  vertex_buffer& operator= (vertex_buffer&& other) = delete;

  void bind(GLenum mode = GL_ARRAY_BUFFER) {
    glBindBuffer(mode, vbo);
  }

  ~vertex_buffer() {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
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

  // do not copy or copy assign
  vertex_array(vertex_array&) = delete;
  vertex_array& operator=(const vertex_array&) = delete;

  //moving is ok
  vertex_array(vertex_array&& old) : vao(old.vao) {
    old.vao = 0;
  }
  // move assigning is not ok
  vertex_array& operator= (vertex_array&& other) = delete;

  void bind() const {
    glBindVertexArray(vao);
  }

  ~vertex_array() {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }
};

template<typename C>
inline std::vector<vector_2f> create_polygon_border(C const& verts, size_t segments, float radius) {
  std::vector<vector_2f> buffer;

  size_t segments_floor = segments / 2;
  bool has_odd_segments = (segments % 2 == 1);
  float segment_angle = math_consts::pi() / segments;

  for (size_t i = 0; i < verts.size(); i++) {
    size_t j = (i + 1) % verts.size();
    vector_2f start = verts[i];
    vector_2f end = verts[j];
    vector_2f norm = (end - start).normalized();
    vector_2f perp = norm.perp();

    if (has_odd_segments) {
      buffer.push_back(start - radius * norm);
    }
    for (size_t k = 0; k < segments_floor; k++) {
      float ang = segment_angle * (k + 1);
      float adj = std::cos(ang);
      float ops = std::sin(ang);

      buffer.push_back(start - adj * radius * norm + ops * radius * perp);
      buffer.push_back(start - adj * radius * norm - ops * radius * perp);  
    }

    buffer.push_back(start + radius * perp);
    buffer.push_back(start - radius * perp);
    buffer.push_back(end + radius * perp);
    buffer.push_back(end - radius * perp);


    for (size_t k = 0; k < segments_floor; k++) {
      float ang = segment_angle * (k + 1);
      float adj = std::cos(ang);
      float ops = std::sin(ang);

      buffer.push_back(end + ops * radius * norm + adj * radius * perp);
      buffer.push_back(end + ops * radius * norm - adj * radius * perp);
    }
    if (has_odd_segments) {
      buffer.push_back(end + radius * norm);
    }
    if (j != 0) {
      // add a degenerate triangle to seperate segments
      buffer.push_back({ buffer.back() });
    }
  }

  return buffer;
}

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

  explicit simple_vertex_array() : size(0) {}

  simple_vertex_array(vertex_buffer vertex_buff, vertex_array vertex_arr, GLuint vertex_count) :
    vb(std::move(vertex_buff)), va(std::move(vertex_arr)), size(vertex_count) {
  }

  // do not copy or copy assign
  simple_vertex_array(simple_vertex_array&) = delete;
  simple_vertex_array& operator=(const simple_vertex_array&) = delete;

  //moving is ok
  simple_vertex_array(simple_vertex_array&& old) : vb(std::move(old.vb)), va(std::move(old.va)), size(old.size) {
  }
  // move assinging is not ok
  simple_vertex_array& operator= (simple_vertex_array&& other) = delete;

  void draw(GLenum mode) const {
    va.bind();
    glDrawArrays(mode, 0, size);
  }
};