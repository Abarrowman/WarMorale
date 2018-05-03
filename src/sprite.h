#pragma once

#include "renderable.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include <functional>

class sprite_context {
public:
  shader* sprite_shader;
  vertex_array* sprite_vertex_array;
  GLint shader_trans_mat_idx;
  GLint shader_proj_mat_idx;
  GLint shader_frame_width_idx;
  GLint shader_frame_height_idx;
  GLint shader_frame_col_idx;
  GLint shader_frame_row_idx;

  void init(shader* s_shader, vertex_array* s_vertex_array) {
    sprite_shader = s_shader;
    sprite_vertex_array = s_vertex_array;
    shader_trans_mat_idx = glGetUniformLocation(sprite_shader->program, "trans_mat");
    shader_proj_mat_idx = glGetUniformLocation(sprite_shader->program, "proj_mat");
    shader_frame_width_idx = glGetUniformLocation(sprite_shader->program, "frame_width");
    shader_frame_height_idx = glGetUniformLocation(sprite_shader->program, "frame_height");
    shader_frame_col_idx = glGetUniformLocation(sprite_shader->program, "frame_col");
    shader_frame_row_idx = glGetUniformLocation(sprite_shader->program, "frame_row");

  }

  void update_projection(matrix_3f const& proj_mat) {
    sprite_shader->use();
    glUniformMatrix3fv(shader_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
  }
};

class sprite : public renderable {
public:

  sprite_context* context;
  texture* tex;
  int frame_width = 1;
  int frame_height = 1;
  int frame_col = 0;
  int frame_row = 0;

  sprite() {}
  sprite(sprite_context* ctx, texture* t) {
    init(ctx, t);
  }

  void init(sprite_context* ctx, texture* t) {
    context = ctx;
    tex = t;
  }


  void render(matrix_3f const& parent_trans) override {
    context->sprite_shader->use();

    tex->activate_bind(GL_TEXTURE0);
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->shader_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());

    glUniform1i(context->shader_frame_width_idx, frame_width);
    glUniform1i(context->shader_frame_height_idx, frame_height);
    glUniform1i(context->shader_frame_col_idx, frame_col);
    glUniform1i(context->shader_frame_row_idx, frame_row);

    context->sprite_vertex_array->draw();
  }

  virtual bool update() override {
    return false;
  }

  virtual ~sprite() {}

};