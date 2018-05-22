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
  //shader* polygon_edge_shader;
  shader* polygon_fill_shader;

  /*GLint edge_trans_mat_idx;
  GLint edge_proj_mat_idx;
  GLint edge_color_idx;*/

  GLint fill_trans_mat_idx;
  GLint fill_proj_mat_idx;
  GLint fill_color_idx;

  void init(shader* fill_shader) {
    polygon_fill_shader = fill_shader;
    fill_trans_mat_idx = glGetUniformLocation(polygon_fill_shader->program, "trans_mat");
    fill_proj_mat_idx = glGetUniformLocation(polygon_fill_shader->program, "proj_mat");
    fill_color_idx = glGetUniformLocation(polygon_fill_shader->program, "color");

  }

  void update_projection(matrix_3f const& proj_mat) {
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

  owning_polygon(polygon_context* ctx, vertex_array va) : context(ctx), arr(std::move(va)) {
  }

  void render(matrix_3f const& parent_trans) {
    if (!visible) {
      return;
    }
    assert(context != nullptr);
    context->polygon_fill_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->fill_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform4fv(context->fill_color_idx, 1, fill_color.floats.data());

    arr.draw(GL_POLYGON);
  }
};