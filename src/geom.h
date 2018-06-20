#pragma once

#include "2d_math.h"
#include <vector>

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


inline std::vector<vector_2f> rotate_verticies(std::vector<vector_2f> src, float angle) {
  matrix_3f rot_max = matrix_3f::rotation_matrix(angle);
  std::vector<vector_2f> result{src.size()};
  for (size_t i = 0; i < src.size(); i++) {
    result[i] = rot_max * src[i];
  }
  return result;
}

class precalc_polygon {
public:
  std::vector<vector_2f> verticies;
private:
  std::vector<edge> edges;

public:
  precalc_polygon() {}

  precalc_polygon(std::vector<vector_2f> verts) : verticies(std::move(verts)) {
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
  polygon_edge_pt point_on_edge(vector_2f center, vector_2f location) {

    vector_2f diff = location - center;
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

    /*vector_2f effective_normal = ed.normal;
    float t = (ed.diff.dot(diff) - ed.diff.dot(ed.first)) / ed.diff.dot(ed.diff);
    if (t > 1.0f) {
    t = 1.0f;
    effective_normal += edges[(edge_idx + 1) % edges.size()].normal;
    } else if (t < 0.0f) {
    t = 0.0f;
    effective_normal -= edges[(edge_idx + edges.size() - 1) % edges.size()].normal;
    }*/

    float t = (ed.diff.dot(diff) - ed.diff.dot(ed.first)) / ed.diff.dot(ed.diff);
    if (t > 1.0f) {
      t = 1.0f;
    } else if (t < 0.0f) {
      t = 0.0f;
    }

    vector_2f edge_pt = center + ed.first + t * ed.diff;

    float proj_distance_to_edge = ed.normal.dot(ed.first) / ed.normal.dot(dir);
    bool inside = proj_distance_to_edge > diff.magnitude();

    return { edge_pt, ed.normal, inside };
  }

};