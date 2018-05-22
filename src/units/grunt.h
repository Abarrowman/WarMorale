#pragma once


#include "../world_face.h"
#include "../team_face.h"
#include "../unit_face.h"

class grunt : public unit {
private:
  sprite* ship;
  //float ang = 0;
public:
  grunt(world& w, team& t, legion* l) : unit(w, t, l) {
    ship = land.static_sprite_orphan(static_texture_id::grunt);
    ship->local_trans = matrix_3f::transformation_matrix(32, 32);
    add_orphan(ship);

  }

protected:
  bool living_update() override {
    unit_reference closest_enemy_ref = find_closest_enemy();

    if (closest_enemy_ref.valid()) {
      unit& closest_enemy = closest_enemy_ref.ref();


      float e_angle = angle_err(trans.angle, trans.translation_to(closest_enemy.trans).angle());
      vector_2f e_pos = trans.translation_to(group->order.pos);

      trans.angle += 0.1f * e_angle;
      trans.set_position(trans.get_position() + e_pos * 0.1f);
    }
    return false;
  }
};

