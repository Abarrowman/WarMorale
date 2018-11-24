#pragma once

#include "../gl_includes.h"
#include "../renderable.h"
#include "../color.h"
#include "bitmap_font.h"

#include <vector>
#include <cassert>

class bitmap_text_context {
private:
  program* text_shader;
  GLint trans_mat_idx;
  GLint color_idx;

public:
  void init(program* t_shader) {
    text_shader = t_shader;
    trans_mat_idx = text_shader->get_uniform_location("trans_mat");
    color_idx = text_shader->get_uniform_location("color");
  }


  static void init_vertex_array(vertex_buffer& vb, vertex_array& va) {
    va.bind();
    glEnableVertexAttribArray(0); // xy
    vb.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1); // st
    vb.bind();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  }

  static void create_char_triangles(
    std::vector<textured_vertex>& buffer,
    vector_2f const& top_left_vert, vector_2f const& bottom_right_vert,
    vector_2f const& top_left_tex, vector_2f const& bottom_right_tex) {

    // First Triangle
      // Top Left
    buffer.emplace_back(top_left_vert, top_left_tex);
    // Bottom Right
    buffer.emplace_back(bottom_right_vert, bottom_right_tex);
    // Top Right
    buffer.emplace_back(
      vector_2f{ bottom_right_vert.x, top_left_vert.y },
      vector_2f{ bottom_right_tex.x, top_left_tex.y });

    // Second Triangle
    // Top Left
    buffer.emplace_back(top_left_vert, top_left_tex);
    // Bottom Left 
    buffer.emplace_back(
      vector_2f{ top_left_vert.x, bottom_right_vert.y },
      vector_2f{ top_left_tex.x, bottom_right_tex.y });
    // Bottom Right
    buffer.emplace_back(bottom_right_vert, bottom_right_tex);
  }

  void render_text_buffer(
      vertex_buffer& vb, vertex_array& va, std::vector<textured_vertex> const& buffer,
      texture& tex, matrix_3f const& parent_trans, matrix_3f const& local_trans,
      color_rgba const& text_color) {
    vb.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(textured_vertex) * buffer.size(), buffer.data(), GL_STREAM_DRAW);

    text_shader->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    glUniform4fv(color_idx, 1, text_color.values.data());

    va.bind();
    tex.activate_bind(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, buffer.size());
  }
};

class mono_bitmap_text : public renderable {
public:
  vertex_buffer vb;
  vertex_array va;

  bitmap_text_context* context;
  mono_bitmap_font* font;
  std::string text;
  color_rgba text_color;

  mono_bitmap_text(bitmap_text_context* ctx, mono_bitmap_font* fo, std::string tex = "") : context(ctx), font(fo), text(std::move(tex)) {
    bitmap_text_context::init_vertex_array(vb, va);
  }

  void render(matrix_3f const& parent_trans) {
    assert(font != nullptr);

    //TODO consider allocating these as part of the class
    // they they wouldn't don't need to be reallocated each render
    std::vector<textured_vertex> buffer;
    buffer.reserve(text.size() * 6);

    vector_2f origin{ 0, 0 };
    for (char c : text) {
      if (c == '\n') {
        origin = { 0, origin.y - font->char_height };
        continue;
      }
      assert(font->contains_char(c));
      vector_2f top_left_vert = origin;
      vector_2f bottom_right_vert{top_left_vert.x + font->char_width, top_left_vert.y - font->char_height};

      vector_2f top_left_tex = font->top_left_tex(c);
      vector_2f bottom_right_tex = top_left_tex + font->char_tex_size;

      bitmap_text_context::create_char_triangles(buffer, top_left_vert, bottom_right_vert, top_left_tex, bottom_right_tex);

      // move origin for next character
      origin.x += font->char_width;
    }

    context->render_text_buffer(vb, va, buffer, font->tex, parent_trans, local_trans, text_color);
  }
};

class prop_bitmap_text : public renderable {
public:
  vertex_buffer vb;
  vertex_array va;

  bitmap_text_context* context;
  prop_bitmap_font* font;
  std::string text;
  color_rgba text_color;

  prop_bitmap_text(bitmap_text_context* ctx,prop_bitmap_font* fo, std::string tex = "") : context(ctx), font(fo), text(std::move(tex)) {
    bitmap_text_context::init_vertex_array(vb, va);
  }

  void render(matrix_3f const& parent_trans) {
    assert(font != nullptr);

    //TODO consider allocating these as part of the class
    // they they wouldn't don't need to be reallocated each render
    std::vector<textured_vertex> buffer;
    buffer.reserve(text.size() * 6);

    vector_2f origin{ 0, 0 };
    for (char c : text) {
      if (c == '\n') {
        origin = { 0, origin.y - font->char_height() };
        continue;
      }
      assert(font->contains_char(c));
      vector_2f top_left_vert = origin;
      int char_width = font->char_width(c);
      vector_2f bottom_right_vert{ top_left_vert.x + char_width, top_left_vert.y - font->char_height() };

      vector_2f top_left_tex = font->top_left_tex(c);
      vector_2f bottom_right_tex{ top_left_tex.x + static_cast<float>(char_width) / font->tex.width(), top_left_tex.y + font->char_tex_size().y };

      bitmap_text_context::create_char_triangles(buffer, top_left_vert, bottom_right_vert, top_left_tex, bottom_right_tex);

      // move origin for next character
      origin.x += char_width;
    }
    context->render_text_buffer(vb, va, buffer, font->tex, parent_trans, local_trans, text_color);
  }
};