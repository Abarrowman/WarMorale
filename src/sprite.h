#pragma once

#include "renderable.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include <functional>

class sprite_context {
public:
  shader const& sprite_shader;
  vertex_array const& sprite_vertex_array;
  GLint shader_trans_mat_idx;
  GLint shader_proj_mat_idx;
  sprite_context(shader& s_shader, vertex_array& s_vertex_array) : sprite_shader(s_shader), sprite_vertex_array(s_vertex_array) {
    shader_trans_mat_idx = glGetUniformLocation(sprite_shader.program, "trans_mat");
    shader_proj_mat_idx = glGetUniformLocation(sprite_shader.program, "proj_mat");
  }

  void update_projection(matrix_3f const& proj_mat) {
    sprite_shader.use();
    glUniformMatrix3fv(shader_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
  }
};

class sprite : public renderable {
public:

  sprite_context context;
  std::reference_wrapper<texture const> tex;

  sprite(sprite_context const& ctx, texture const& t) : context(ctx), tex(t) {}


  void render(matrix_3f const& parent_trans) override {
    context.sprite_shader.use();
    tex.get().activate_bind(GL_TEXTURE0);
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context.shader_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    context.sprite_vertex_array.draw();
  }

  virtual bool update() override {
    return false;
  }

  virtual ~sprite() {}

};