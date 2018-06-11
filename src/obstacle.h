#pragma once

#include "renderable.h"
#include "sprite.h"
#include "potential_field.h"
#include "space_buckets.h"
#include "polygon.h"
#include <array>

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

  virtual vector_2f get_exerted_gradient(vector_2f effected_location, float other_radius) = 0;
  virtual ~obstacle() {} // base class
};

class circular_obstacle : public obstacle {
private:
  float radius;

public:
  circular_obstacle(float rad, sprite* orphan, polygon_context* p_ctx) : radius(rad) {
    /*std::array<vector_2f, 2>verts{ { { 0.0f, 0.0f },{ 0.0f, 0.0001f } } };
    simple_vertex_array arr = simple_vertex_array::create_verticies(std::move(verts));
    add_orphan(new owning_polygon(p_ctx, std::move(arr)))->edge_color = { 0.0f, 1.0f, 1.0f, 1.0f };*/
    add_orphan(orphan);

  }

  vector_2f get_exerted_gradient(vector_2f location, float other_radius) {
    vector_2f gauss_force = 5.0f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, radius - 40.0f);
    //vector_2f obs_force = 000.f * normalized_absolute_obstacle_gradient(trans.get_position(), location, radius + other_radius, radius);
    //return gauss_force + obs_force;
    return gauss_force;
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

  vector_2f get_exerted_gradient(vector_2f location, float radius) {
    std::vector<obstacle*> nearby = buckets.find_nearby_buckets(location);
    vector_2f grad = vector_2f::zero();
    for (obstacle* ob_ptr : nearby) {
      grad += ob_ptr->get_exerted_gradient(location, radius);
    }
    return grad;
  }
};