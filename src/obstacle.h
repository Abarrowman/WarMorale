#pragma once

#include "renderable.h"
#include "sprite.h"
#include "potential_field.h"
#include "space_buckets.h"
#include "polygon.h"
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

  virtual vector_2f get_exerted_gradient(vector_2f effected_location, float other_radius) = 0;
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

  vector_2f get_exerted_gradient(vector_2f location, float other_radius) override {
    vector_2f gauss_force;
    if (radius >= 40.0f) {
      gauss_force = 3.5f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, radius - 40.0f);
    } else {
      gauss_force = 3.5f * normalized_gaussian_gradient(trans.get_position(), location, (other_radius + radius) / 2.0f);
    }
    return gauss_force;
  }

  void render(matrix_3f const& parent_trans) override {
    //variadic_render(parent_trans, *this, poly, image);
    //variadic_render(parent_trans, *this, poly);
    variadic_render(parent_trans, *this, image, poly);
  }
};

struct polygon_edge_pt {
  vector_2f pt;
  vector_2f normal;
  bool inside;
};

struct edge {
  vector_2f first;
  vector_2f last;
  vector_2f diff;
  vector_2f normal;
  float first_angle;
  float last_angle;
};

class polygonal_obstacle : public obstacle {
private:
  std::vector<vector_2f> verticies;
  std::vector<edge> edges;
  sprite image;
  owning_polygon poly;
public:
  polygonal_obstacle(std::vector<vector_2f> verts, sprite img, polygon_context* p_ctx) :
      verticies(std::move(verts)), image(std::move(img)), poly(p_ctx, simple_vertex_array::create_verticies(verticies)) {
    poly.edge_width = 1.0f;
    poly.edge_color = color::white();
    poly.fill_color = color::transparent_black();

    edges.resize(verticies.size());
    for (size_t i = 0; i < verticies.size(); i++) {
      edge& ed = edges[i];
      ed.first = verticies[i];
      ed.last = verticies[(i + 1) % verticies.size()];
      ed.diff = ed.last - ed.first;
      ed.normal = ed.diff.perp();

      ed.first_angle = positive_angle(ed.first.angle());
      ed.last_angle = positive_angle(ed.last.angle());
    }
  }

  // Returns the closest point on the surface and if the point is inside the polygon
  polygon_edge_pt point_on_edge(vector_2f location) {

    vector_2f diff = location - trans.get_position();
    vector_2f dir = diff.normalized();
    float angle = positive_angle(diff.angle());

    size_t edge_idx = edges.size();
    edge ed;
    for (size_t i = 0; i < edges.size(); i++) {
      edge& cur_ed = edges[i];
      bool in_range = false;
      if (cur_ed.last_angle > cur_ed.first_angle) {
        in_range = (angle >= cur_ed.first_angle) && (angle <cur_ed.last_angle);
      } else {
        // angle overflow case
        // last_angle is in the 1st quadrant
        // first_angle is in the 3rd or 4th quadrant
        in_range = (angle < cur_ed.last_angle) || (angle > cur_ed.last_angle);
      }
      if (in_range) {
        ed = cur_ed;
        edge_idx = i;
        break;
      }
    }
    assert(edge_idx != edges.size());

    vector_2f effective_normal = ed.normal;
    
    float t = (ed.diff.dot(diff) - ed.diff.dot(ed.first)) / ed.diff.dot(ed.diff);
    if (t > 1.0f) {
      t = 1.0f;
      effective_normal += edges[(edge_idx + 1) % edges.size()].normal;
    } else if (t < 0.0f) {
      t = 0.0f;
      effective_normal -= edges[(edge_idx + edges.size() - 1) % edges.size()].normal;
    }
    vector_2f edge_pt = trans.get_position() + ed.first + t * ed.diff;

    float dist = ed.normal.dot(ed.first) / ed.normal.dot(dir);

    return { edge_pt, effective_normal, dist > diff.magnitude() };
  }

  vector_2f get_exerted_gradient(vector_2f location, float other_radius) override {
    polygon_edge_pt edge_pt = point_on_edge(location);
    float sign = edge_pt.inside ? -1.0f : 1.0f;
    

    //vector_2f raw_gauss_force = sign * 4.0f * normalized_gaussian_gradient(edge_pt.pt, location, other_radius / 2.0f + 15.0f);

    vector_2f gauss_force = sign * 2.0f * normalized_gaussian_gradient(edge_pt.pt, location, other_radius + 20.0f);
    vector_2f obs_force = sign * 2.0f * normalized_fractional_obstacle_gradient(edge_pt.pt, location, other_radius + 20.0f);
    vector_2f raw_gauss_force = gauss_force + obs_force;

    //vector_2f raw_gauss_force = sign * 3.5f * normalized_gaussian_gradient(edge_pt.pt, location, other_radius / 2.0f + 10.0f);
    //vector_2f raw_gauss_force = sign * 0.8f * normalized_gaussian_gradient(edge_pt.pt, location, other_radius / 2.0f + 10.0f);


    //vector_2f edge_diff = edge_pt.pt - trans.get_position();
    //float edge_radius = edge_diff.magnitude();
    //vector_2f raw_gauss_force = 3.5f * normalized_absolute_gaussian_gradient(trans.get_position(), location, other_radius / 2.0f + 20.0f, edge_radius - 40);

    /*vector_2f edge_diff = edge_pt.pt - trans.get_position();
    float edge_radius = edge_diff.magnitude();
    vector_2f raw_gauss_force = sign * 3.5f * normalized_absolute_gaussian_gradient(edge_pt.pt, location, other_radius / 2.0f + 20.0f, 0);*/


    //vector_2f gauss_force = raw_gauss_force;
    vector_2f final_force = raw_gauss_force.proj(edge_pt.normal);

    //vector_2f gauss_force = vector_2f::zero();
    //return final_force;
    return raw_gauss_force;
  }

  void render(matrix_3f const& parent_trans) override {
    //variadic_render(parent_trans, *this, poly, image);
    //variadic_render(parent_trans, *this, poly);
    variadic_render(parent_trans, *this, image, poly);
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