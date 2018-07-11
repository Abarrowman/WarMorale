#pragma once

#include "2d_math.h"
#include "utils.h"
#include <vector>
#include <cassert>
#include <algorithm>

enum class geom_orientation {
  colinear,
  clockwise,
  counter_clockwise
};


struct polygon_edge_pt {
  vector_2f pt;
  vector_2f normal;
  bool contains;
};

struct bounds {
  vector_2f min_bound;
  vector_2f max_bound;

  bool contains(vector_2f pt) const {
    return (
      (pt.x >= min_bound.x) &&
      (pt.y >= min_bound.y) &&
      (pt.x <= max_bound.x) &&
      (pt.y <= max_bound.y));
  }
};

struct precalc_segment {
  vector_2f first;
  vector_2f last;
  vector_2f dir;

  precalc_segment() {}
  precalc_segment(vector_2f fir, vector_2f las) :
    first(fir),
    last(las),
    dir(last - first) {}
  precalc_segment(vector_2f fir, vector_2f las, vector_2f di) :
    first(fir),
    last(las),
    dir(di) {
  }


  bounds get_bounds() const {
    return { {std::min(first.x, last.x), std::min(first.y, last.y)},
    { std::max(first.x, last.x), std::max(first.y, last.y) } };
  }

  precalc_segment operator-(vector_2f const& other) const {
    return precalc_segment(first - other, last - other, dir);
  }
};

struct precalc_edge {
  vector_2f first;
  vector_2f last;
  vector_2f diff;
  vector_2f normal;
  float first_angle;
  float last_angle;

  precalc_edge() {}
  precalc_edge(vector_2f fir, vector_2f las) :
    first(fir),
    last(las),
    diff(last - first),
    normal(diff.perp()),
    first_angle(positive_angle(first.angle())),
    last_angle(positive_angle(last.angle())) {}

  precalc_segment get_segment() const {
    return precalc_segment(first, last, diff);
  }
};


inline std::vector<vector_2f> rotate_verticies(std::vector<vector_2f> src, float angle) {
  matrix_3f rot_max = matrix_3f::rotation_matrix(angle);
  std::vector<vector_2f> result{src.size()};
  for (size_t i = 0; i < src.size(); i++) {
    result[i] = rot_max * src[i];
  }
  return result;
}


inline geom_orientation triangle_orientation(vector_2f p, vector_2f q, vector_2f r) {
  float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (float_equal(val, 0.0f)) {
    return geom_orientation::colinear;
  }
  if (val > 0) {
    return geom_orientation::clockwise;
  } else {
    return geom_orientation::counter_clockwise;
  }
}

inline float point_to_segment_distance(vector_2f pt, precalc_segment segment) {
  vector_2f diff = pt - segment.first;
  float t = std::clamp(diff.dot(segment.dir) / segment.dir.dot(segment.dir), 0.0f, 1.0f);
  vector_2f closest_point_on_segment = segment.first + t * segment.dir;
  return (pt - closest_point_on_segment).magnitude();
}

inline bool do_segments_intersect(precalc_segment seg, precalc_segment other) {

  geom_orientation o1 = triangle_orientation(seg.first, seg.last, other.first);
  geom_orientation o2 = triangle_orientation(seg.first, seg.last, other.last);
  geom_orientation o3 = triangle_orientation(other.first, other.last, seg.first);
  geom_orientation o4 = triangle_orientation(other.first, other.last, seg.last);

  // general case
  if ((o1 != o2) && (o3 != o4)) {
    return true;
  }

  // Colinear special cases
  if ((o1 == geom_orientation::colinear) && seg.get_bounds().contains(other.first)) {
    return true;
  }
  if ((o2 == geom_orientation::colinear) && seg.get_bounds().contains(other.last)) {
    return true;
  }
  if ((o3 == geom_orientation::colinear) && other.get_bounds().contains(seg.first)) {
    return true;
  }
  if ((o4 == geom_orientation::colinear) && other.get_bounds().contains(seg.last)) {
    return true;
  }
  return false;
}

inline float segment_to_segment_distance(precalc_segment seg, precalc_segment other) {
  if (do_segments_intersect(seg, other)) {
    return 0.0f;
  }

  // do not intersect shortest distance is one of 4 points
  float seg_first_dis = point_to_segment_distance(seg.first, other);
  float seg_last_dis = point_to_segment_distance(seg.last, other);
  float other_first_dis = point_to_segment_distance(other.first, seg);
  float other_last_dis = point_to_segment_distance(other.last, seg);

  return std::min(seg_first_dis, std::min(seg_last_dis, std::min(other_first_dis, other_last_dis)));
}

class precalc_polygon {
public:
  std::vector<vector_2f> verticies;
private:
  std::vector<precalc_edge> edges;

public:
  precalc_polygon() {}

  precalc_polygon(std::vector<vector_2f> verts) : verticies(std::move(verts)) {
    edges.resize(verticies.size());
    for (size_t i = 0; i < verticies.size(); i++) {
      edges[i] = precalc_edge(verticies[i], verticies[(i + 1) % verticies.size()]);
    }
  }

  // Returns the closest point on the surface and if the point is contains the polygon
  polygon_edge_pt point_on_edge(vector_2f loc, float inside_tolerance = 0.0f) {

    vector_2f dir = loc.normalized();
    float angle = positive_angle(loc.angle());

    size_t edge_idx = edges.size();
    precalc_edge ed;
    for (size_t i = 0; i < edges.size(); i++) {
      precalc_edge& cur_ed = edges[i];
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

    float t = (ed.diff.dot(loc) - ed.diff.dot(ed.first)) / ed.diff.dot(ed.diff);
    if (t > 1.0f) {
      t = 1.0f;
    } else if (t < 0.0f) {
      t = 0.0f;
    }

    vector_2f edge_pt = ed.first + t * ed.diff;

    float proj_distance_to_edge = ed.normal.dot(ed.first) / ed.normal.dot(dir);
    bool contains = proj_distance_to_edge > (loc.magnitude() - inside_tolerance);

    return { edge_pt, ed.normal, contains };
  }

  bool is_point_occupied(vector_2f loc, float inside_tolerance = 0.0f) {
    return point_on_edge(loc, inside_tolerance).contains;
  }

  float distance_to_segment(precalc_segment segment) {
    float min_distance = std::numeric_limits<float>::max();
    for (precalc_edge& e : edges) {
      min_distance = std::min(segment_to_segment_distance(segment, e.get_segment()), min_distance);
    }
    return min_distance;
  }


  bool is_segment_occupied(precalc_segment segment, float inside_tolerance = 0.0f) {
    // TODO unit test
    // O(n) check if start is in the polygon
    // O(n) check if finish is in the polygon
    // O(n) check the distance between each precalc_edge and the line segment from start to finish
    // Potential optimization(s)
    // if the distance from the line to the center of the polygon is within the incirlce radius + other_radius the line is occupied O(1)
    // if the distance from the line to the center of the polygon is otuside the maximum radius of the polygon + other_radius the line is not occupied O(1)
    if (is_point_occupied(segment.first, inside_tolerance) || is_point_occupied(segment.last, inside_tolerance)) {
      return true;
    }
    return (distance_to_segment(segment) <= inside_tolerance);
  }

};