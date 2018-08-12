#pragma once

#include "renderable.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include "color.h"
#include <functional>
#include <cassert>

class sprite;

class sprite_context {
public:
  shader* sprite_shader;
  simple_vertex_array* sprite_vertex_array;
  GLint trans_mat_idx;
  GLint proj_mat_idx;
  GLint frames_idx;
  GLint current_frame_idx;
  GLint mask_color;


  void init(shader* s_shader, simple_vertex_array* s_vertex_array) {
    sprite_shader = s_shader;
    sprite_vertex_array = s_vertex_array;
    trans_mat_idx = sprite_shader->get_uniform_location("trans_mat");
    frames_idx = sprite_shader->get_uniform_location("frames");
    current_frame_idx = sprite_shader->get_uniform_location("current_frame");
    mask_color = sprite_shader->get_uniform_location("mask_color");
  }

  sprite* create_orphan(texture* t);
  sprite create(texture* t);
};

class sprite : public renderable {
public:

  sprite_context* context;
  texture* tex;


  vector_2i frames{ 1, 1 };
  vector_2i current_frame{ 0, 0 };

  color_rgba mask_color;

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
    glUniformMatrix3fv(context->trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform2i(context->frames_idx, frames.x, frames.y);
    glUniform2i(context->current_frame_idx, current_frame.x, current_frame.y);
    glUniform4fv(context->mask_color, 1, mask_color.values.data());

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
