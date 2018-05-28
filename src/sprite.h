#pragma once

#include "renderable.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include <functional>

class sprite;

class sprite_context {
public:
  shader* sprite_shader;
  simple_vertex_array* sprite_vertex_array;
  GLint shader_trans_mat_idx;
  GLint shader_proj_mat_idx;
  GLint shader_frame_width_idx;
  GLint shader_frame_height_idx;
  GLint shader_frame_col_idx;
  GLint shader_frame_row_idx;

  void init(shader* s_shader, simple_vertex_array* s_vertex_array) {
    sprite_shader = s_shader;
    sprite_vertex_array = s_vertex_array;
    shader_trans_mat_idx = sprite_shader->get_uniform_location("trans_mat");
    shader_proj_mat_idx = sprite_shader->get_uniform_location("proj_mat");
    shader_frame_width_idx = sprite_shader->get_uniform_location("frame_width");
    shader_frame_height_idx = sprite_shader->get_uniform_location("frame_height");
    shader_frame_col_idx = sprite_shader->get_uniform_location("frame_col");
    shader_frame_row_idx = sprite_shader->get_uniform_location("frame_row");

  }

  void update_projection(matrix_3f const& proj_mat) {
    sprite_shader->use();
    glUniformMatrix3fv(shader_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
  }

  sprite* create_orphan(texture* t);
  sprite create(texture* t);
};

class sprite : public renderable {
public:

  sprite_context* context;
  texture* tex;
  int frame_width = 1;
  int frame_height = 1;
  int frame_col = 0;
  int frame_row = 0;

  sprite() {
    context = nullptr;
    tex = nullptr;
  }
  sprite(sprite_context* ctx, texture* t) {
    init(ctx, t);
  }

  void init(sprite_context* ctx, texture* t) {
    context = ctx;
    tex = t;
  }


  void render(matrix_3f const& parent_trans) override {
    if (!visible) {
      return;
    }
    assert(context != nullptr);
    context->sprite_shader->use();

    assert(tex != nullptr);
    tex->activate_bind(GL_TEXTURE0);
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->shader_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());

    glUniform1i(context->shader_frame_width_idx, frame_width);
    glUniform1i(context->shader_frame_height_idx, frame_height);
    glUniform1i(context->shader_frame_col_idx, frame_col);
    glUniform1i(context->shader_frame_row_idx, frame_row);

    context->sprite_vertex_array->draw(GL_TRIANGLES);
  }

  virtual ~sprite() {}

};

inline sprite* sprite_context::create_orphan(texture* t) {
    return new sprite(this, t);
}

inline sprite sprite_context::create(texture * t) {
  return sprite(this, t);
}
