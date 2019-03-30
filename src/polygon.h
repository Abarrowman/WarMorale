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

    polygon_edge_program = fill_shader;
    edge_trans_mat_idx = polygon_edge_program->get_uniform_location("trans_mat");
    edge_color_idx = polygon_edge_program->get_uniform_location("color");
  }
};

template<typename P>
inline void render_polygon(P const& p, matrix_3f const& parent_trans, simple_vertex_array const& fill_arr, simple_vertex_array const& border_arr) {
  if (!p.visible) {
    return;
  }
  assert(p.context != nullptr);

  matrix_3f full_trans = parent_trans * p.local_trans;

  p.context->polygon_fill_program->use();
  glUniformMatrix3fv(p.context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
  glUniform4fv(p.context->fill_color_idx, 1, p.fill_color.values.data());
  fill_arr.draw(GL_POLYGON);

  p.context->polygon_edge_program->use();
  glUniformMatrix3fv(p.context->edge_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
  glUniform4fv(p.context->edge_color_idx, 1, p.edge_color.values.data());
  border_arr.draw(GL_TRIANGLE_STRIP);
}


class sharing_polygon : public renderable {
public:
  polygon_context* context;
  simple_vertex_array const* fill_arr;
  simple_vertex_array const* border_arr;

  color_rgba fill_color;
  color_rgba edge_color;

  sharing_polygon(polygon_context* ctx, simple_vertex_array* fa, simple_vertex_array* ba) :
    context(ctx), fill_arr(fa), border_arr(ba) {}

  void render(matrix_3f const& parent_trans) {
    assert(fill_arr != nullptr);
    assert(border_arr != nullptr);
    render_polygon(*this, parent_trans, *fill_arr, *border_arr);
  }
};

class owning_polygon : public renderable {
public:
  polygon_context* context;
  simple_vertex_array fill_arr;
  simple_vertex_array border_arr;

  color_rgba fill_color;
  color_rgba edge_color;

  owning_polygon(polygon_context* ctx, simple_vertex_array fa, simple_vertex_array ba) :
    context(ctx), fill_arr(std::move(fa)), border_arr(std::move(ba)) {}

  template<typename C>
  owning_polygon(polygon_context* ctx, C const& verts, float edge_radius) :
    context(ctx),
    fill_arr(simple_vertex_array::create_verticies(verts)),
    border_arr(simple_vertex_array::create_verticies(create_polygon_border(verts, 5, edge_radius))) {}

  void render(matrix_3f const& parent_trans) {
    render_polygon(*this, parent_trans, fill_arr, border_arr);
  }

  template<typename C>
  void set_veritices(C const& verts, float edge_radius) {
    fill_arr.set_veritices(verts);
    border_arr.set_veritices(create_polygon_border(verts, 5, edge_radius));
  }
};