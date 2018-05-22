#pragma once

#include "matrix_3f.h"
#include "team_face.h"
#include "unit_face.h"

inline unit::unit(world& w, team& t, legion* l) : land(w), side(t), group(l) {
  group->add_unit(this);
}

inline unit::~unit() {
  //fprintf(stderr, "Unit Destructor\n");
  if (group) {
    group->remove_unit(this);
  }
}

inline bool unit::update() {
  assert(group != nullptr);
  switch (status) {
  case LIVING:
    if (living_update()) {
      visible = false;
      status = unit_status::KILLED;
    }
    break;
  case KILLED:
    status = DYING;
    break;
  case DYING:
    return true;
    break;
  }
  ordered_parent::update();
  trans.clamp_angle();
  local_trans = trans.to_matrix();
  return false;
}

inline unit_reference unit::find_closest_enemy() {
  unit_reference closest_enemy{nullptr};

  float closest_dist = std::numeric_limits<float>::max();
  for (team* enemy_side : side.get_enemies()) {
    int enemy_count = enemy_side->child_count();
    for (int n = 0; n < enemy_count; n++) {
      unit& enemy = enemy_side->child_at(n);
      if (!enemy.is_living()) {
        continue;
      }
      float enemy_dist = enemy.trans.translation_to(trans).magnitude();
      if (enemy_dist < closest_dist) {
        closest_dist = enemy_dist;
        closest_enemy = unit_reference(enemy);
      }
    }
  }

  return closest_enemy;
}

inline bool unit::is_living() {
  return (status == LIVING);
}

inline void unit_reference::update() {
  if (target != nullptr) {
    if (!(target->is_living())) {
      target = nullptr;
    }
  }
}

inline bool unit_reference::valid() {
  update();
  return (target != nullptr);
}

inline unit* unit_reference::ptr() {
  update();
  return target;
}

inline unit& unit_reference::ref() {
  update();
  assert(target != nullptr);
  return *target;
}