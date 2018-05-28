#pragma once

#include "utils.h"
#include "renderable.h"
#include "fixed_width_bitmap_font.h"
#include "color.h"
#include <vector>

class fixed_width_bitmap_text_context {
public:
  shader* text_shader;
  GLint shader_trans_mat_idx;
  GLint shader_proj_mat_idx;
  GLint shader_char_width;
  GLint shader_char_height;
  GLint shader_texture_width;
  GLint shader_texture_height;
  GLint shader_color;

  void init(shader* t_shader) {
    text_shader = t_shader;
    shader_trans_mat_idx = text_shader->get_uniform_location("trans_mat");
    shader_proj_mat_idx = text_shader->get_uniform_location("proj_mat");
    shader_char_width = text_shader->get_uniform_location("char_width");
    shader_char_height = text_shader->get_uniform_location("char_height");
    shader_texture_width = text_shader->get_uniform_location("texture_width");
    shader_texture_height = text_shader->get_uniform_location("texture_height");
    shader_color = text_shader->get_uniform_location("color");
  }

  void update_projection(matrix_3f const& proj_mat) {
    text_shader->use();
    glUniformMatrix3fv(shader_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
  }
};

class fixed_width_bitmap_text : public renderable {
public:
  vertex_buffer corner_buffer;
  vertex_buffer character_buffer;
  vertex_array va;

  fixed_width_bitmap_text_context* context;
  fixed_width_bitmap_font* font;
  std::string text;
  color text_color;

  fixed_width_bitmap_text(fixed_width_bitmap_text_context* ctx, fixed_width_bitmap_font* fo, std::string tex = ""): context(ctx), font(fo), text(std::move(tex)) {
    va.bind();

    glEnableVertexAttribArray(0);
    corner_buffer.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    character_buffer.bind();
    glVertexAttribIPointer(1, 1, GL_INT, 1 * sizeof(int), 0);
  }

  void render(matrix_3f const& parent_trans) {
    assert(font != nullptr);

    //TODO consider allocating these as part of the class
    // they they wouldn't don't need to be reallocated each render
    std::vector<vector_2f> corners;
    std::vector<int> characters;

    corners.reserve(text.size());
    characters.reserve(text.size());


    vector_2f origin{0, 0};
    for (char c : text) {
      if (c == '\n') {
        origin = {0, origin.y - font->char_height};
        continue;
      }
      assert(font->contains_char(c));
      vector_2f top_left_char_corner = origin;
      corners.push_back(top_left_char_corner);
      //characters.push_back(0xffff00ff);
      characters.push_back(static_cast<int>(c));
      origin.x += font->char_width;
    }

    int render_char_count = corners.size();

    corner_buffer.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * render_char_count * 2, corners.data(), GL_STREAM_DRAW);

    character_buffer.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * render_char_count, characters.data(), GL_STREAM_DRAW);

    context->text_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->shader_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform1i(context->shader_char_width, font->char_width);
    glUniform1i(context->shader_char_height, font->char_height);
    glUniform1i(context->shader_texture_width, font->tex.width);
    glUniform1i(context->shader_texture_height, font->tex.height);
    glUniform4fv(context->shader_color, 1, text_color.floats.data());

    va.bind();
    font->tex.activate_bind(GL_TEXTURE0);
    glDrawArrays(GL_POINTS, 0, render_char_count);

  }
};