#pragma once
#include "threat_face.h"
#include "obstacle.h"

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
  }
}

bool point_threat::update(world& world_ref) {
  trans.set_position(trans.get_position() + velocity);
  if (world_ref.obstacle_layer->is_occupied(trans.get_position(), 0)) {
    destroyed = true;
  }
  if (lifetime > 0) {
    lifetime--;
  } else if (lifetime == 0) {
    destroyed = true;
  }
  if (destroyed) {
    vector_2f center = trans.get_position();
    matrix_3f parent_trans = trans.to_matrix();
    world_ref.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(world_ref.pp_ctx), image, center, parent_trans, world_ref.get_generator(), 20));
  }
  return destroyed;
}

void point_threat::render(matrix_3f const& parent_trans) {
  variadic_trans_render(parent_trans, *this, image);
}