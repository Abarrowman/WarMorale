#pragma once

#include "2d_math.h"
#include "team_face.h"
#include "unit_face.h"
#include <limits>

inline void unit::take_threats() {

  std::array<std::vector<threat*>*, 9> threat_vec_arr = world_ref.threat_layer->get_nearby_threats(trans.get_position());
  for (std::vector<threat*>* threat_vec_ptr : threat_vec_arr) {
    if (threat_vec_ptr == nullptr) {
      continue;
    }
    std::vector<threat*>& threat_vec = *threat_vec_ptr;
    for (threat* t : threat_vec) {
      t->hurt(*this);
    }
  }
}

inline bool unit::take_point_threat(point_threat& pt) {
  vector_2f dis = trans.translation_to(pt.trans);
  if (dis.magnitude() < type.potential_radius) {
    current_health -= pt.damage;
    return true;
  } else {
    return false;
  }
}

inline unit::unit(world& w, team& t, legion* l, unit_archetype const& ty) : world_ref(w), team_ref(t), legion_ptr(l), type(ty) {
  legion_ptr->add_unit(this);
  current_health = type.max_health;
}

inline unit::~unit() {
  //fprintf(stderr, "Unit Destructor\n");
  if (legion_ptr) {
    legion_ptr->remove_unit(this);
  }
}

inline void unit::living_update() {
  vector_2f dest = legion_ptr->order.pos;
  vector_2f position = trans.get_position();
  vector_2f diff = dest - position;
  float distance = diff.magnitude();

  std::array<std::vector<unit_reference>*, 9> ref_arr = world_ref.unit_buckets.find_nearby_buckets(position);


  //vector_2f grad = vector_2f::zero();
  vector_2f goal_grad = legion_ptr->order.get_potential_force(trans.get_position());// *(type.potential_radius / 16.0f);
  vector_2f grad = goal_grad;
  for (std::vector<unit_reference>* ref_vec_ptr : ref_arr) {
    if (ref_vec_ptr == nullptr) {
      continue;
    }
    std::vector<unit_reference>& ref_vec = *ref_vec_ptr;
    for (unit_reference ref : ref_vec) {
      if (!ref.valid()) {
        continue;
      }
      if (ref.ptr() == this) {
        continue;
      }
      unit& close_unit = ref.ref();

      float intersection_radius = close_unit.type.potential_radius + type.potential_radius;
      vector_2f gauss_force = 0.8f * normalized_gaussian_gradient(close_unit.trans.get_position(), position, 0.5f * intersection_radius);
      vector_2f obs_force = 0.4f * normalized_fractional_obstacle_gradient(close_unit.trans.get_position(), position, intersection_radius);
      grad += gauss_force + obs_force;
    }
  }

  vector_2f obs_force = world_ref.obstacle_layer->get_exerted_gradient(position, type.potential_radius);
  grad += obs_force;

  float mag = grad.magnitude();
  if (mag != 0) {
    vector_2f capped_gradient = (std::min(type.max_speed, mag) / mag) * grad;
    trans.set_position(trans.get_position() + capped_gradient);
  }

  if (current_reload > 0) {
    current_reload--;
  }

  unit_reference closest_enemy_ref = find_closest_enemy();
  if (closest_enemy_ref.valid()) {
    unit& closest_enemy = closest_enemy_ref.ref();

    vector_2f diff = trans.translation_to(closest_enemy.trans);
    float e_angle = angle_err(trans.angle, diff.angle());
    //float angle_diff += 0.1f * e_angle;
    float angle_diff = absolute_value_clamp(type.max_turn_speed, e_angle);
    trans.angle += angle_diff;


    if (current_reload == 0) {
      float rem_angle_err = e_angle - angle_diff;
      vector_2f dir = vector_2f::create_polar(trans.angle);
      vector_2f closest_point = trans.get_position() + diff.dot(dir) * dir;
      float distance = (closest_point - closest_enemy.trans.get_position()).magnitude();

      if ((std::abs(rem_angle_err) < math_consts::pi() * 0.25) && (distance < closest_enemy.type.potential_radius)) {
        //fire
        sprite bullet_sprite = world_ref.static_sprite(static_texture_id::shot);
        bullet_sprite.alpha = 0.5f;
        bullet_sprite.local_trans = matrix_3f::transformation_matrix(16, 16, trans.angle);
        point_threat* bullet = world_ref.threat_layer->add_orphan(new point_threat(bullet_sprite, 1, dir * 5.0f, 100, &team_ref));
        bullet->trans.set_position(trans.get_position() + dir * type.potential_radius);
        current_reload = type.max_reload;
      }
    }

    
    //
    //float cross_err = diff.magnitude() * std::sin(rem_angle_err);
  }
}

inline bool unit::update() {
  assert(legion_ptr != nullptr);
  switch (status) {
  case LIVING:
    world_ref.unit_buckets.remove_entry(old_pos, ref());
    living_update();
    take_threats();
    if (current_health <= 0) {
      death_action();
      visible = false;
      status = unit_status::KILLED;
    } else {
      trans.clamp_angle();
      world_ref.unit_buckets.add_entry(trans.get_position(), ref());
      old_pos = trans.get_position();
    }
    break;
  case KILLED:
    status = DYING;
    break;
  case DYING:
    return true;
    break;
  }
  
  return false;
}

inline unit_reference unit::find_closest_enemy() {
  unit_reference closest_enemy{nullptr};

  float closest_dist = std::numeric_limits<float>::max();
  for (team* enemy_side : team_ref.get_enemies()) {
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

inline unit_reference unit::find_closest_enemy(std::vector<unit_reference>& references) {
  unit_reference closest_enemy{ nullptr };
  float closest_dist = std::numeric_limits<float>::max();
  for (unit_reference ref : references) {
    if (!ref.valid()) {
      continue;
    }
    unit& other = ref.ref();
    if (!team_ref.is_hositle(&(other.team_ref))) {
      continue;
    }
    float enemy_dist = other.trans.translation_to(trans).magnitude();
    if (enemy_dist < closest_dist) {
      closest_dist = enemy_dist;
      closest_enemy = ref;
    }
  }
  return closest_enemy;
}

inline bool unit::is_living() {
  return (status == LIVING);
}

inline unit_reference unit::ref() {
  return unit_reference(this);
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

inline bool unit_reference::operator==(unit_reference const & other) const {
  return (target == other.target);
}
