#pragma once


#include "../world_face.h"
#include "../team_face.h"
#include "../unit_face.h"
#include "../threat.h"
#include "../potential_field.h"

class grunt : public unit {
private:
  sprite* ship;
public:
  grunt(world& w, team& t, legion* l) : unit(w, t, l, 10) {
    ship = add_orphan(land.static_sprite_orphan(static_texture_id::grunt));
    ship->local_trans = matrix_3f::transformation_matrix(32, 32);
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


    //vector_2f grad = vector_2f::zero();
    vector_2f grad = quadratic_cone_gradient(dest, trans.get_position(), 100);
    std::vector<unit_reference> references = land.unit_buckets.find_nearby_buckets(position);
    for (unit_reference ref : references) {
      if (!ref.valid()) {
        continue;
      }
      if (ref.ptr() == this) {
        continue;
      }
      unit& close_unit = ref.ref();

      vector_2f repulse = 20.0f * downwards_quadratic_gradient(close_unit.trans.get_position(), position, close_unit.pot_radius + pot_radius);
      grad -= repulse;
    }


    float mag = grad.magnitude();
    float max_speed = 10.0f;
    if (mag != 0) {
      vector_2f capped_gradient = (std::min(std::min(distance, max_speed), mag) / mag) * grad;
      trans.set_position(trans.get_position() - capped_gradient);
    }

     /*// move
    vector_2f e_pos = trans.translation_to(group->order.pos);
    trans.set_position(position + e_pos * 0.1f);*/

    // aim
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
    land.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(land.pp_ctx), *ship, center, parent_trans, land.get_generator()));
  }
};

