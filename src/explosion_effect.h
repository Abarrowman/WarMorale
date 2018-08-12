#pragma once

#include "renderable.h"
#include "2d_math.h"
#include "vertex_array.h"
#include "color.h"
#include "sprite.h"
#include <vector>
#include <cassert>

class point_particle_context {
public:
  shader* point_particle_shader;
  GLint shader_trans_mat_idx;

  void init(shader* p_shader) {
    point_particle_shader = p_shader;
    shader_trans_mat_idx = point_particle_shader->get_uniform_location("trans_mat");

  }
};

class explosion_effect : public renderable {
private:
  point_particle_context * context;
  int particle_count;
  int duration;
  std::vector<vector_2f> positions;
  std::vector<color_rgba> colors;
  std::vector<vector_2f> velocities;

  vertex_buffer position_buffer;
  vertex_buffer color_buffer;
  vertex_array va;
  float alpha_step;
public:

  static explosion_effect* create_example_orphan(point_particle_context* ctx, vector_2f center) {
    int particle_count = 10000;
    std::vector<vector_2f> positions;
    std::vector<color_rgba> colors;
    std::vector<vector_2f> velocities;
    positions.resize(particle_count);
    colors.resize(particle_count);
    velocities.resize(particle_count);

    for (int i = 0; i < particle_count; i++) {
      int x = i % 100;
      float x_frac = (x + 1) / 100.0f;
      float x_cen = x_frac * 2.0f - 1.0f;
      int y = i / 100;
      float y_frac = (y + 1) / 100.0f;
      float y_cen = y_frac * 2.0f - 1.0f;
      positions[i] = center + vector_2f(x_cen * 50, y_cen * 50);
      colors[i] = { x_frac, y_frac, 0.0f, 1.0f };
      velocities[i] = { x_cen, y_cen };
    }
    return new explosion_effect(ctx, particle_count, 100, std::move(positions), std::move(colors), std::move(velocities), 0.01f);
  }

  static explosion_effect* explode_sprite(point_particle_context* ctx,  sprite& spr, vector_2f center, matrix_3f const& parent_trans, generator_type& gen, int duration=60) {
    assert(spr.tex != nullptr);

    int const image_width = spr.tex->width;
    int const image_height = spr.tex->height;
    std::vector<unsigned char> pixel_data;
    pixel_data.resize(4 * image_width * image_height);

    glGetTextureImage(spr.tex->tex, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.size(), pixel_data.data());

    int max_particle_count = image_height * image_width;
    vector_2f image_center = { static_cast<float>(image_width - 1) / 2.0f, static_cast<float>(image_height - 1) / 2.0f };
    std::vector<vector_2f> positions;
    std::vector<color_rgba> colors;
    std::vector<vector_2f> velocities;
    positions.reserve(max_particle_count);
    colors.reserve(max_particle_count);
    velocities.reserve(max_particle_count);

    matrix_3f total_trans = parent_trans * spr.local_trans;



    vector_2i frame_size{ image_width, image_height };
    frame_size /= spr.frames;

    vector_2i offset = frame_size * spr.current_frame;

    for (int x = 0; x < frame_size.x; x++) {
      for (int y = 0; y < frame_size.y; y++) {

        int tx = x + offset.x;
        int ty = y + offset.y;
        unsigned char a_val = pixel_data[(ty * image_width + tx) * 4 + 3];
        if (a_val != 0) {
          unsigned char r_val = pixel_data[(ty * image_width + tx) * 4 + 0];
          unsigned char g_val = pixel_data[(ty * image_width + tx) * 4 + 1];
          unsigned char b_val = pixel_data[(ty * image_width + tx) * 4 + 2];

          color_rgba p_col = { r_val / 255.0f, g_val / 255.0f, b_val / 255.0f, a_val / 255.0f };
          p_col *= spr.mask_color;

          //TODO add to p_col to make explosions look better
          vector_2f pos = vector_2f(static_cast<float>(x) / (frame_size.x - 1) - 0.5f, static_cast<float>(y) / (frame_size.y - 1) - 0.5f);
          vector_2f trans_pos = total_trans * pos;

          positions.push_back(trans_pos);
          colors.push_back(p_col);
          velocities.push_back((trans_pos - center) * 0.5f * rand_float(gen));
        }
      }
    }
    int particle_count = positions.size();

    return new explosion_effect(ctx, particle_count, duration, std::move(positions), std::move(colors), std::move(velocities), spr.mask_color.values[3] / duration);
  }

  explosion_effect(point_particle_context* ctx, int count, int dur, std::vector<vector_2f> pos,
      std::vector<color_rgba> cols, std::vector<vector_2f> vels, float al_step) : context(ctx), particle_count(count),
      duration(dur), positions(std::move(pos)), colors(std::move(cols)), velocities(std::move(vels)), alpha_step(al_step) {

    assert(positions.size() == particle_count);
    assert(colors.size() == particle_count);
    assert(velocities.size() == particle_count);


    va.bind();

    glEnableVertexAttribArray(0);
    position_buffer.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    color_buffer.bind();
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  }

  // do not copy or assign
  explosion_effect(explosion_effect&) = delete;
  explosion_effect& operator=(const explosion_effect&) = delete;

  bool update() override {
    for (int i = 0; i < particle_count; i++) {
      positions[i] += velocities[i];
      velocities[i] *= 0.9f;
      colors[i].values[3] -= alpha_step;
    }
    duration--;
    return (duration < 0);
  }

  void render(matrix_3f const& parent_trans) override {
    if (!visible) {
      return;
    }
    assert(context != nullptr);

    position_buffer.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particle_count * 2, positions.data(), GL_STREAM_DRAW);
    color_buffer.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particle_count * 4, colors.data(), GL_STREAM_DRAW);

    context->point_particle_shader->use();

    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->shader_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());

    va.bind();
    glDrawArrays(GL_POINTS, 0, particle_count);
  }
};