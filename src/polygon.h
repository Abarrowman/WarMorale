#pragma once

#include "utils.h"
#include "renderable.h"
#include "shader.h"
#include "vertex_array.h"

class color {
public:
  std::array<float, 4> floats = { 1, 1, 1, 1 };
};

class polygon_context {
public:
  shader * polygon_fill_shader;
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
    fill_trans_mat_idx = glGetUniformLocation(polygon_fill_shader->program, "trans_mat");
    fill_proj_mat_idx = glGetUniformLocation(polygon_fill_shader->program, "proj_mat");
    fill_color_idx = glGetUniformLocation(polygon_fill_shader->program, "color");


    polygon_edge_shader = edge_shader;
    edge_trans_mat_idx = glGetUniformLocation(polygon_edge_shader->program, "trans_mat");
    edge_proj_mat_idx = glGetUniformLocation(polygon_edge_shader->program, "proj_mat");
    edge_width_idx = glGetUniformLocation(polygon_edge_shader->program, "width");
    edge_color_idx = glGetUniformLocation(polygon_edge_shader->program, "color");
    edge_cap_type_idx = glGetUniformLocation(polygon_edge_shader->program, "cap_type");
  }

  void update_projection(matrix_3f const& proj_mat) {
    polygon_edge_shader->use();
    glUniformMatrix3fv(edge_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
    polygon_fill_shader->use();
    glUniformMatrix3fv(fill_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());

  }
};

/*class sharing_polygon : public renderable {
public:
  polygon_context* context;
  vertex_array* arr;
  color fill_color;

  void render(matrix_3f const& parent_trans) {
    if (!visible) {
      return;
    }
    assert(context != nullptr);

    context->polygon_fill_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform4fv(context->fill_color_idx, 1, fill_color.floats.data());

    assert(arr != nullptr);
    arr->draw();
  }
};*/

class owning_polygon : public renderable {
public:
  polygon_context* context;
  vertex_array arr;

  color fill_color;
  color edge_color;
  float edge_width = 1;

  owning_polygon(polygon_context* ctx, vertex_array va) : context(ctx), arr(std::move(va)) {
  }

  void render(matrix_3f const& parent_trans) {
    if (!visible) {
      return;
    }
    assert(context != nullptr);

    matrix_3f full_trans = parent_trans * local_trans;

    context->polygon_fill_shader->use();
    glUniformMatrix3fv(context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform4fv(context->fill_color_idx, 1, fill_color.floats.data());
    arr.draw(GL_POLYGON);

    context->polygon_edge_shader->use();
    glUniformMatrix3fv(context->edge_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform4fv(context->edge_color_idx, 1, edge_color.floats.data());
    glUniform1f(context->edge_width_idx, edge_width);
    glUniform1i(context->edge_cap_type_idx, 2);
    arr.draw(GL_LINE_LOOP);
  }
};