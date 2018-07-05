#pragma once

#include "utils.h"
#include "renderable.h"
#include "shader.h"
#include "vertex_array.h"
#include "color.h"

class polygon_context {
public:
  shader* polygon_fill_shader;
  GLint fill_trans_mat_idx;
  GLint fill_proj_mat_idx;
  GLint fill_color_idx;

  shader* polygon_edge_shader;
  GLint edge_trans_mat_idx;
  GLint edge_proj_mat_idx;
  GLint edge_width_idx;
  GLint edge_color_idx;
  GLint edge_cap_type_idx;


  void init(shader* fill_shader, shader* edge_shader) {
    polygon_fill_shader = fill_shader;
    fill_trans_mat_idx = polygon_fill_shader->get_uniform_location("trans_mat");
    fill_proj_mat_idx = polygon_fill_shader->get_uniform_location("proj_mat");
    fill_color_idx = polygon_fill_shader->get_uniform_location("color");


    polygon_edge_shader = edge_shader;
    edge_trans_mat_idx = polygon_edge_shader->get_uniform_location("trans_mat");
    edge_proj_mat_idx = polygon_edge_shader->get_uniform_location("proj_mat");
    edge_width_idx = polygon_edge_shader->get_uniform_location("width");
    edge_color_idx = polygon_edge_shader->get_uniform_location("color");
    edge_cap_type_idx = polygon_edge_shader->get_uniform_location("cap_type");
  }

  void update_projection(matrix_3f const& proj_mat) {
    polygon_edge_shader->use();
    glUniformMatrix3fv(edge_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
    polygon_fill_shader->use();
    glUniformMatrix3fv(fill_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());

  }
};

template<typename P>
inline void render_polygon(P const& p, matrix_3f const& parent_trans, simple_vertex_array const& arr) {
  if (!p.visible) {
    return;
  }
  assert(p.context != nullptr);

  matrix_3f full_trans = parent_trans * p.local_trans;

  p.context->polygon_fill_shader->use();
  glUniformMatrix3fv(p.context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
  glUniform4fv(p.context->fill_color_idx, 1, p.fill_color.values.data());
  arr.draw(GL_POLYGON);

  p.context->polygon_edge_shader->use();
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