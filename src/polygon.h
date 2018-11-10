#pragma once

#include "gl_includes.h"
#include "renderable.h"
#include "shader.h"
#include "vertex_array.h"
#include "color.h"
#include <cassert>

class polygon_context {
public:
  program* polygon_fill_program;
  GLint fill_trans_mat_idx;
  GLint fill_color_idx;

  program* polygon_edge_program;
  GLint edge_trans_mat_idx;
  GLint edge_width_idx;
  GLint edge_color_idx;
  GLint edge_cap_type_idx;


  void init(program* fill_shader, program* edge_shader) {
    polygon_fill_program = fill_shader;
    fill_trans_mat_idx = polygon_fill_program->get_uniform_location("trans_mat");
    fill_color_idx = polygon_fill_program->get_uniform_location("color");

    polygon_edge_program = edge_shader;
    edge_trans_mat_idx = polygon_edge_program->get_uniform_location("trans_mat");
    edge_width_idx = polygon_edge_program->get_uniform_location("width");
    edge_color_idx = polygon_edge_program->get_uniform_location("color");
    edge_cap_type_idx = polygon_edge_program->get_uniform_location("cap_type");
  }
};

template<typename P>
inline void render_polygon(P const& p, matrix_3f const& parent_trans, simple_vertex_array const& arr) {
  if (!p.visible) {
    return;
  }
  assert(p.context != nullptr);

  matrix_3f full_trans = parent_trans * p.local_trans;

  p.context->polygon_fill_program->use();
  glUniformMatrix3fv(p.context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
  glUniform4fv(p.context->fill_color_idx, 1, p.fill_color.values.data());
  arr.draw(GL_POLYGON);

  p.context->polygon_edge_program->use();
  glUniformMatrix3fv(p.context->edge_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
  glUniform4fv(p.context->edge_color_idx, 1, p.edge_color.values.data());
  glUniform1f(p.context->edge_width_idx, p.edge_width);
  glUniform1i(p.context->edge_cap_type_idx, 2);
  arr.draw(GL_LINE_LOOP);
}


class sharing_polygon : public renderable {
public:
  polygon_context* context;
  simple_vertex_array const* arr;

  color_rgba fill_color;
  color_rgba edge_color;
  float edge_width = 1;

  sharing_polygon(polygon_context* ctx, simple_vertex_array* va) : context(ctx), arr(va) {
  }

  void render(matrix_3f const& parent_trans) {
    assert(arr != nullptr);
    render_polygon(*this, parent_trans, *arr);
  }
};

class owning_polygon : public renderable {
public:
  polygon_context* context;
  simple_vertex_array arr;

  color_rgba fill_color;
  color_rgba edge_color;
  float edge_width = 1;

  owning_polygon(polygon_context* ctx, simple_vertex_array va) : context(ctx), arr(std::move(va)) {
  }

  void render(matrix_3f const& parent_trans) {
    render_polygon(*this, parent_trans, arr);
  }
};