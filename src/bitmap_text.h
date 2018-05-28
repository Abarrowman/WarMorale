#pragma once

#include "utils.h"
#include "renderable.h"
#include "fixed_width_bitmap_font.h"
#include "color.h"
#include <vector>

class bitmap_text_context {
public:
  shader* mono_shader;
  GLint mono_trans_mat_idx;
  GLint mono_proj_mat_idx;
  GLint mono_char_width;
  GLint mono_char_height;
  GLint mono_texture_width;
  GLint mono_texture_height;
  GLint mono_color;

  shader* prop_shader;
  GLint prop_trans_mat_idx;
  GLint prop_proj_mat_idx;
  GLint prop_char_height;
  GLint prop_texture_width;
  GLint prop_texture_height;
  GLint prop_color;

  void init(shader* m_shader, shader* p_shader) {
    mono_shader = m_shader;
    mono_trans_mat_idx = mono_shader->get_uniform_location("trans_mat");
    mono_proj_mat_idx = mono_shader->get_uniform_location("proj_mat");
    mono_char_width = mono_shader->get_uniform_location("char_width");
    mono_char_height = mono_shader->get_uniform_location("char_height");
    mono_texture_width = mono_shader->get_uniform_location("texture_width");
    mono_texture_height = mono_shader->get_uniform_location("texture_height");
    mono_color = mono_shader->get_uniform_location("color");

    prop_shader = p_shader;
    prop_trans_mat_idx = prop_shader->get_uniform_location("trans_mat");
    prop_proj_mat_idx = prop_shader->get_uniform_location("proj_mat");
    prop_char_height = prop_shader->get_uniform_location("char_height");
    prop_texture_width = prop_shader->get_uniform_location("texture_width");
    prop_texture_height = prop_shader->get_uniform_location("texture_height");
    prop_color = prop_shader->get_uniform_location("color");
  }

  void update_projection(matrix_3f const& proj_mat) {
    mono_shader->use();
    glUniformMatrix3fv(mono_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
    prop_shader->use();
    glUniformMatrix3fv(prop_proj_mat_idx, 1, GL_TRUE, proj_mat.values.data());
  }
};

class fixed_width_bitmap_text : public renderable {
public:
  vertex_buffer corner_buffer;
  vertex_buffer character_buffer;
  vertex_array va;

  bitmap_text_context* context;
  fixed_width_bitmap_font* font;
  std::string text;
  color text_color;

  fixed_width_bitmap_text(bitmap_text_context* ctx, fixed_width_bitmap_font* fo, std::string tex = ""): context(ctx), font(fo), text(std::move(tex)) {
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

    context->mono_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->mono_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform1i(context->mono_char_width, font->char_width);
    glUniform1i(context->mono_char_height, font->char_height);
    glUniform1i(context->mono_texture_width, font->tex.width);
    glUniform1i(context->mono_texture_height, font->tex.height);
    glUniform4fv(context->mono_color, 1, text_color.floats.data());

    va.bind();
    font->tex.activate_bind(GL_TEXTURE0);
    glDrawArrays(GL_POINTS, 0, render_char_count);

  }
};

class variable_width_bitmap_text : public renderable {
public:
  vertex_buffer vb;
  vertex_array va;


  bitmap_text_context* context;
  variable_width_bitmap_font* font;
  std::string text;
  color text_color;

  variable_width_bitmap_text(bitmap_text_context* ctx, variable_width_bitmap_font* fo, std::string tex = "") : context(ctx), font(fo), text(std::move(tex)) {
    va.bind();
    vb.bind();

    glEnableVertexAttribArray(0); // corner
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1); // corner texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(2); // width
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(4 * sizeof(float)));
  }

  void render(matrix_3f const& parent_trans) {
    assert(font != nullptr);

    //TODO consider allocating these as part of the class
    // they they wouldn't don't need to be reallocated each render
    std::vector<float> vert_data;
    vert_data.reserve(text.size() * 5);

    int char_count = 0;
    vector_2f origin{ 0, 0 };
    for (char c : text) {
      if (c == '\n') {
        origin = { 0, origin.y - font->char_height() };
        continue;
      }
      assert(font->contains_char(c));

      vector_2f top_left_vert = origin;
      vert_data.push_back(top_left_vert.x);
      vert_data.push_back(top_left_vert.y);

      vector_2f top_left_texture = font->char_texture_top_left_coordinates(c);
      vert_data.push_back(top_left_texture.x);
      vert_data.push_back(top_left_texture.y);

      int char_width = font->char_width(c);
      vert_data.push_back(static_cast<float>(char_width));

      origin.x += char_width;
      char_count++;
    }


    vb.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * char_count, vert_data.data(), GL_STREAM_DRAW);

    context->prop_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->prop_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform1i(context->prop_char_height, font->char_height());
    glUniform1i(context->prop_texture_width, font->tex.width);
    glUniform1i(context->prop_texture_height, font->tex.height);
    glUniform4fv(context->prop_color, 1, text_color.floats.data());

    va.bind();
    font->tex.activate_bind(GL_TEXTURE0);
    glDrawArrays(GL_POINTS, 0, char_count);

  }
};