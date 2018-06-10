#pragma once

#include "renderable.h"
#include "sprite.h"
#include "potential_field.h"
#include "space_buckets.h"

class obstacle_parent;

class obstacle : public ordered_parent {
  friend obstacle_parent;
private:
  trans_state old_trans;
public:
  trans_state trans;

  virtual bool update() override {
    local_trans = old_trans.to_matrix();
    return ordered_parent::update();
  }

  virtual vector_2f get_exerted_gradient(vector_2f effected_location) = 0;
  virtual ~obstacle() {} // base class
};

class circular_obstacle : public obstacle {
private:
  float radius;

public:
  circular_obstacle(float rad, sprite* orphan) : radius(rad) {
    add_orphan(orphan);
  }

  vector_2f get_exerted_gradient(vector_2f location) {
    float half_radius = radius * 0.5f;
    vector_2f personal_space_force = -45000.0f * half_radius * half_radius * gaussian_gradient(trans.get_position(), location, half_radius);
   // vector_2f collision_avoidance_force = -100000000.0f * obstacle_gradient(trans.get_position(), location, radius);
    //return personal_space_force + collision_avoidance_force;
    return personal_space_force;
  }
};

class obstacle_parent : public renderable_parent<obstacle, true> {
private:
  using parent_type = renderable_parent<obstacle, true>;
  space_buckets<obstacle*> buckets{ 400 };

public:

  bool update() {
    bool result = parent_type::update();
    for (int n = 0; n < child_count(); n++) {
      obstacle& ob = child_at(n);
      if (ob.old_trans != ob.trans) {
        buckets.move_entry(ob.old_trans.get_position(), ob.trans.get_position(), &ob);
        ob.old_trans = ob.trans;
        ob.local_trans = ob.trans.to_matrix();
      }
    }
    return result;
  }

  vector_2f get_exerted_gradient(vector_2f location) {
    std::vector<obstacle*> nearby = buckets.find_nearby_buckets(location);
    vector_2f grad = vector_2f::zero();
    for (obstacle* ob_ptr : nearby) {
      grad += ob_ptr->get_exerted_gradient(location);
    }
    return grad;
  }
};