#pragma once


#include "../world_face.h"
#include "../team_face.h"
#include "../unit_face.h"
#include "../threat.h"

class grunt : public unit {
private:
  sprite* ship;
public:
  grunt(world& w, team& t, legion* l) : unit(w, t, l, 10) {
    ship = land.static_sprite_orphan(static_texture_id::grunt);
    ship->local_trans = matrix_3f::transformation_matrix(32, 32);
    add_orphan(ship);
  }

  bool take_point_threat(point_threat& pt) override {
    vector_2f dis = trans.translation_to(pt.trans);
    if (dis.magnitude() < 16) {
      current_health -= pt.damage;
      return true;
    } else {
      return false;
    }
  }


protected:
  void living_update() override {
    unit_reference closest_enemy_ref = find_closest_enemy();

    if (closest_enemy_ref.valid()) {
      unit& closest_enemy = closest_enemy_ref.ref();


      float e_angle = angle_err(trans.angle, trans.translation_to(closest_enemy.trans).angle());
      vector_2f e_pos = trans.translation_to(group->order.pos);

      trans.angle += 0.1f * e_angle;
      trans.set_position(trans.get_position() + e_pos * 0.1f);
    }
  }

  void death_action() override {
    assert(ship->tex != nullptr);

    int const image_width = ship->tex->width;
    int const image_height = ship->tex->height;
    std::vector<unsigned char> pixel_data;
    pixel_data.resize(4 * image_width * image_height);

    glGetTextureImage(ship->tex->tex, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.size(), pixel_data.data());

    int max_particle_count = image_height * image_width;
    vector_2f image_center = { static_cast<float>(image_width - 1) / 2.0f, static_cast<float>(image_height - 1) / 2.0f };
    std::vector<vector_2f> positions;
    std::vector<color> colors;
    std::vector<vector_2f> velocities;
    positions.reserve(max_particle_count);
    colors.reserve(max_particle_count);
    velocities.reserve(max_particle_count);

    vector_2f center = trans.get_position();
    matrix_3f total_trans = trans.to_matrix() * ship->local_trans;

    for (int x = 0; x < image_width; x++) {
      for (int y = 0; y < image_height; y++) {
        unsigned char a_val = pixel_data[(y * image_width + x) * 4 + 3];
        if (a_val != 0) {
          unsigned char r_val = pixel_data[(y * image_width + x) * 4 + 0];
          unsigned char g_val = pixel_data[(y * image_width + x) * 4 + 1];
          unsigned char b_val = pixel_data[(y * image_width + x) * 4 + 2];

          color p_col = { r_val / 255.0f, g_val / 255.0f, b_val / 255.0f, a_val / 255.0f};
          vector_2f pos = vector_2f(static_cast<float>(x) / (image_width - 1) - 0.5f, static_cast<float>(y) / (image_height - 1) - 0.5f);
          vector_2f trans_pos = total_trans * pos;

          positions.push_back(trans_pos);
          colors.push_back(p_col);
          velocities.push_back((trans_pos - center) * 0.5f);
        }
      }
    }
    int particle_count = positions.size();

    explosion_effect* boom = new explosion_effect(&land.pp_ctx, particle_count, 20, std::move(positions), std::move(colors), std::move(velocities));
    land.over_effects_layer->add_orphan(boom);
  }
};

