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
    vector_2f center = trans.get_position();
    matrix_3f parent_trans = trans.to_matrix();
    land.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(land.pp_ctx), *ship, center, parent_trans, land.get_generator()));
  }
};

