#pragma once


#include "../world_face.h"
#include "../team_face.h"
#include "../unit_face.h"
#include "../threat.h"
#include "../potential_field.h"

class grunt : public unit {
private:
  sprite ship;
  sharing_polygon poly;
  std::vector<unit_reference> references;

public:
  grunt(world& w, team& t, legion* l) :
      unit(w, t, l, 10),
      poly(&(land.p_ctx), &(land.static_res.get_vertex_array(static_vertex_array_id::dodecagon))) {
    ship = land.static_sprite(static_texture_id::grunt);
    ship.local_trans = matrix_3f::transformation_matrix(32, 32);
    poly.edge_width = 1.0f / pot_radius;
    poly.local_trans = matrix_3f::transformation_matrix(pot_radius, pot_radius);
    poly.edge_color = side.col;
    poly.fill_color = color::black(0.0f);
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
    vector_2f dest = group->order.pos;
    vector_2f position = trans.get_position();
    vector_2f diff = dest - position;
    float distance = diff.magnitude();

    references.clear();
    land.unit_buckets.find_nearby_buckets(position, references);


    //vector_2f grad = vector_2f::zero();
    vector_2f grad = 0.1f *  quadratic_cone_gradient(dest, trans.get_position(), 100);
    for (unit_reference ref : references) {
      if (!ref.valid()) {
        continue;
      }
      if (ref.ptr() == this) {
        continue;
      }
      unit& close_unit = ref.ref();

      float intersection_radius = close_unit.pot_radius + pot_radius;
      vector_2f gauss_force = 0.8f * normalized_gaussian_gradient(close_unit.trans.get_position(), position, 0.5f * intersection_radius);
      vector_2f obs_force =  0.4f * normalized_fractional_obstacle_gradient(close_unit.trans.get_position(), position, intersection_radius);
      grad += gauss_force + obs_force;
    }

    vector_2f obs_force = land.obstacle_layer->get_exerted_gradient(position, pot_radius);
    grad += obs_force;

    float mag = grad.magnitude();
    if (mag != 0) {
      vector_2f capped_gradient = (std::min(max_speed, mag) / mag) * grad;
      trans.set_position(trans.get_position() + capped_gradient);
    }
    unit_reference closest_enemy_ref = find_closest_enemy();
    if (closest_enemy_ref.valid()) {
      unit& closest_enemy = closest_enemy_ref.ref();
      float e_angle = angle_err(trans.angle, trans.translation_to(closest_enemy.trans).angle());
      trans.angle += 0.1f * e_angle;
    }

  }

  void death_action() override {
    vector_2f center = trans.get_position();
    matrix_3f parent_trans = trans.to_matrix();
    land.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(land.pp_ctx), ship, center, parent_trans, land.get_generator()));
  }

  void render(matrix_3f const& parent_trans) override {
    if (visible) {
      matrix_3f matrix = parent_trans * trans.to_matrix();
      inner_variadic_render(matrix, ship, poly);
    }
  }
};

