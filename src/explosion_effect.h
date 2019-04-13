#pragma once

#include "renderable.h"
#include "2d_math.h"
#include "vertex_array.h"
#include "color.h"
#include "sprite.h"
#include <vector>
#include <cassert>

struct pt_particle {
  vector_2f pos;
  vector_2f vel;
  color_rgba col;

  pt_particle() {}
  pt_particle(vector_2f p, vector_2f v, color_rgba c) : pos(p), vel(v), col(c) {}
};

class point_particle_context {
public:
  program* rend_prog;
  GLint render_trans_mat_idx;

  program* comp_prog;
  GLint comp_pt_count_idx;
  GLint comp_alpha_step_idx;

  void init(program* render_shader, program* comp_shader) {
    rend_prog = render_shader;
    render_trans_mat_idx = rend_prog->get_uniform_location("trans_mat");

    comp_prog = comp_shader;
    comp_pt_count_idx = comp_prog->get_uniform_location("pt_count");
    comp_alpha_step_idx = comp_prog->get_uniform_location("alpha_step");
  }
};

class explosion_effect : public renderable {
private:
  point_particle_context * context;
  simple_vertex_array sva;
  int duration;
  float alpha_step;
public:

  static explosion_effect* create_example_orphan(point_particle_context* ctx, vector_2f center) {
    int particle_count = 10000;

    std::vector<pt_particle> parts;
    parts.resize(particle_count);

    for (int i = 0; i < particle_count; i++) {
      int x = i % 100;
      float x_frac = (x + 1) / 100.0f;
      float x_cen = x_frac * 2.0f - 1.0f;
      int y = i / 100;
      float y_frac = (y + 1) / 100.0f;
      float y_cen = y_frac * 2.0f - 1.0f;
      parts[i] = { center + vector_2f(x_cen * 50, y_cen * 50) , vector_2f{ x_cen, y_cen },  color_rgba{ x_frac, y_frac, 0.0f, 1.0f } };
    }
    return new explosion_effect(ctx, std::move(parts), 100, 0.01f);
  }

  static explosion_effect explode_sprite(point_particle_context* ctx,  sprite& spr, vector_2f center, matrix_3f const& parent_trans, generator_type& gen, int duration=60) {
    assert(spr.tex != nullptr);

    unsigned char* pixel_data = spr.tex->data();
    vector_2i image_size{ spr.tex->width(), spr.tex->height() };

    vector_2i frame_size = image_size / spr.frames;
    int max_particle_count = frame_size.x * frame_size.y;
    std::vector<pt_particle> parts;
    parts.reserve(max_particle_count);

    matrix_3f total_trans = parent_trans * spr.local_trans;

    vector_2i offset = frame_size * spr.current_frame;

    for (int x = 0; x < frame_size.x; x++) {
      for (int y = 0; y < frame_size.y; y++) {

        int tx = x + offset.x;
        int ty = y + offset.y;
        unsigned char a_val = pixel_data[(ty * image_size.x + tx) * 4 + 3];
        if (a_val != 0) {
          unsigned char r_val = pixel_data[(ty * image_size.x + tx) * 4 + 0];
          unsigned char g_val = pixel_data[(ty * image_size.x + tx) * 4 + 1];
          unsigned char b_val = pixel_data[(ty * image_size.x + tx) * 4 + 2];

          color_rgba p_col = { r_val / 255.0f, g_val / 255.0f, b_val / 255.0f, a_val / 255.0f };
          p_col *= spr.mask_color;

          //TODO add to p_col to make explosions look better
          vector_2f pos = vector_2f(static_cast<float>(x) / (frame_size.x - 1) - 0.5f, static_cast<float>(y) / (frame_size.y - 1) - 0.5f);
          vector_2f trans_pos = total_trans * pos;

          parts.emplace_back(trans_pos, (trans_pos - center) * 0.3f * rand_float(gen), p_col);
        }
      }
    }

    return explosion_effect{ ctx, std::move(parts), duration, spr.mask_color.values[3] / duration };
  }

  explosion_effect(point_particle_context* ctx, std::vector<pt_particle> parts, int dur, float al_step) :
      context(ctx),
      duration(dur),
      alpha_step(al_step) {

    sva.size = parts.size();
    sva.vb.bind(GL_SHADER_STORAGE_BUFFER);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(pt_particle) * sva.size, parts.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sva.vb.vbo);

    sva.va.bind();
    glEnableVertexAttribArray(0);
    sva.vb.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    sva.vb.bind();
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(4 * sizeof(float)));
  }

  // do not copy, assign, or move assign
  explosion_effect(explosion_effect&) = delete;
  explosion_effect& operator=(const explosion_effect&) = delete;
  explosion_effect& operator= (explosion_effect&& old) = delete;

  // moving is ok
  explosion_effect(explosion_effect&& old) :
    context(old.context),
    sva(std::move(old.sva)),
    duration(old.duration),
    alpha_step(old.alpha_step)
  {}

  bool update() override {
    duration--;
    return (duration < 0);
  }

  void render(matrix_3f const& parent_trans) override {
    if (!visible) {
      return;
    }
    assert(context != nullptr);

    context->comp_prog->use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sva.vb.vbo);
    glUniform1ui(context->comp_pt_count_idx, sva.size);
    glUniform1f(context->comp_alpha_step_idx, alpha_step);
    glDispatchCompute(sva.size / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    context->rend_prog->use();
    matrix_3f full_trans = parent_trans * local_trans;
    glUniformMatrix3fv(context->render_trans_mat_idx, 1, GL_TRUE, full_trans.values.data());
    sva.draw(GL_POINTS);
  }
};