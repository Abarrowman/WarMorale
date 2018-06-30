#pragma once

#include "renderable.h"
#include "sprite.h"
#include "potential_field.h"
#include "space_buckets.h"
#include "polygon.h"
#include "geom.h"
#include <array>

class obstacle_parent;

class obstacle : public renderable {
  friend obstacle_parent;
private:
  trans_state old_trans;
public:
  trans_state trans;

  virtual bool update() override {
    local_trans = old_trans.to_matrix();
    return false;
  }

  virtual bool is_occupied(vector_2f location, float other_radius) = 0;
  virtual vector_2f get_exerted_gradient(vector_2f location, float other_radius) = 0;
  virtual ~obstacle() {} // base class
};

class circular_obstacle : public obstacle {
private:
  float radius;
  sprite image;
  sharing_polygon poly;
public:
  circular_obstacle(float rad, sprite img, sharing_polygon pol) : radius(rad), image(std::move(img)), poly(std::move(pol)) {
    assert(radius > 0.0f);
    poly.edge_width = 1.0f / radius;
    poly.local_trans = matrix_3f::transformation_matrix(radius, radius);
    poly.edge_color = color::white();
    poly.fill_color = color::transparent_black();
  }

  bool is_occupied(vector_2f location, float other_radius) override {
    vector_2f diff = trans.translation_to(location);
    return (diff.magnitude() < (radius + other_radius));
  }

  vector_2f get_exerted_gradient(vector_2f location, float other_radius) override {
    vector_2f gauss_force;
    /*if (radius >= 40.0f) {
      gauss_force = 3.5f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, radius - 40.0f);
    } else {
      gauss_force = 3.5f * normalized_gaussian_gradient(trans.get_position(), location, (other_radius + radius) / 2.0f);
    }*/
    if (radius >= 80.0f) {
      gauss_force = 3.0f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, radius - 40.0f);
    } else if (radius >= 40.0f) {
      gauss_force = 2.5f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, radius - 40.0f);
    } else {
      gauss_force = 2.5f * normalized_gaussian_gradient(trans.get_position(), location, (other_radius + radius) / 2.0f);
    }

    return gauss_force;
  }

  void render(matrix_3f const& parent_trans) override {
    //variadic_local_render(parent_trans, *this, poly, image);
    //variadic_local_render(parent_trans, *this, poly);
    variadic_local_render(parent_trans, *this, image, poly);
  }
};

class polygonal_obstacle : public obstacle {
private:
  precalc_polygon precalc;
  sprite image;
  owning_polygon poly;
public:
  polygonal_obstacle(std::vector<vector_2f> verts, sprite img, polygon_context* p_ctx) :
    precalc(std::move(verts)), image(std::move(img)), poly(p_ctx, simple_vertex_array::create_verticies(precalc.verticies)) {
    poly.edge_width = 1.0f;
    poly.edge_color = color::white();
    poly.fill_color = color::transparent_black();
  }

  vector_2f get_exerted_gradient(vector_2f location, float other_radius) override {
    polygon_edge_pt edge_pt = precalc.point_on_edge(trans.get_position(), location);
    float sign = edge_pt.inside ? -1.0f : 1.0f;
    //vector_2f gauss_force = sign * 2.0f * normalized_gaussian_gradient(edge_pt.pt, location, other_radius + 20.0f);
    //vector_2f obs_force = sign * 2.0f * normalized_fractional_obstacle_gradient(edge_pt.pt, location, other_radius + 20.0f);

    if (edge_pt.inside) {
      sign = sign;
    }

    vector_2f gauss_force = sign * 3.0f * normalized_absolute_gaussian_gradient(edge_pt.pt, location, other_radius / 2.0f + 20.0f, -20.0f);

    vector_2f raw_force = gauss_force;

    vector_2f final_force = raw_force;
    //vector_2f final_force = raw_force.proj(edge_pt.normal);
    return final_force;
  }

  bool is_occupied(vector_2f location, float other_radius) override {
    return precalc.point_on_edge(trans.get_position(), location, other_radius).inside;
  }

  void render(matrix_3f const& parent_trans) override {
    //variadic_local_render(parent_trans, *this, poly, image);
    //variadic_local_render(parent_trans, *this, poly);
    variadic_local_render(parent_trans, *this, image, poly);
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
    vector_2f grad = vector_2f::zero();
    std::array<std::vector<obstacle*>*, 9> obstacle_vec_arr = buckets.find_nearby_buckets(location);
    for (std::vector<obstacle*>* obstacle_vec_ptr : obstacle_vec_arr) {
      if (obstacle_vec_ptr == nullptr) {
        continue;
      }
      std::vector<obstacle*>& nearby = *obstacle_vec_ptr;
      for (obstacle* ob_ptr : nearby) {
        grad += ob_ptr->get_exerted_gradient(location, radius);
      }
    }
    return grad;
  }

  bool is_occupied(vector_2f location, float other_radius) {
    std::array<std::vector<obstacle*>*, 9> obstacle_vec_arr = buckets.find_nearby_buckets(location);
    for (std::vector<obstacle*>* obstacle_vec_ptr : obstacle_vec_arr) {
      if (obstacle_vec_ptr == nullptr) {
        continue;
      }
      std::vector<obstacle*>& nearby = *obstacle_vec_ptr;
      for (obstacle* ob_ptr : nearby) {
        if (ob_ptr->is_occupied(location, other_radius)) {
          return true;
        }
      }
    }
    return false;
  }
};