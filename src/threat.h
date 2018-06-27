#pragma once
#include "threat_face.h"

void point_threat::hurt(unit& target) {
  if (destroyed) {
    return;
  }
  if (&(target.team_ref) == allegiance) {
    return;
  }
  if (target.take_point_threat(*this)) {
    // TODO make threats that don't destroy themselves when used
    destroyed = true;


    vector_2f center = trans.get_position();
    matrix_3f parent_trans = trans.to_matrix();
    target.world_ref.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(target.world_ref.pp_ctx), image, center, parent_trans, target.world_ref.get_generator()));
  }
}

bool point_threat::update() {
  trans.set_position(trans.get_position() + velocity);
  if (lifetime > 0) {
    lifetime--;
  } else if (lifetime == 0) {
    return true;
  }
  return destroyed;
}

void point_threat::render(matrix_3f const& parent_trans) {
  variadic_trans_render(parent_trans, *this, image);
}