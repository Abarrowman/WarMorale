#pragma once

#include "gl_includes.h"
#include "team_face.h"
#include "unit_face.h"
#include "potential_field.h"
#include <cassert>

inline vector_2f command::get_potential_force(vector_2f location) {
  if (formation.verticies.size() == 0) {
    return 0.1f *  quadratic_cone_gradient(pos, location, 100);
  } else {
    polygon_edge_pt edge_pt = formation.point_on_edge(location - pos);
    if (edge_pt.contains) {
      return vector_2f::zero();
    } else {
      return 0.1f *  quadratic_cone_gradient(edge_pt.pt + pos, location, 100);
    }
  }
}

inline void legion::add_unit(unit* member) {
  units.push_back(member);
}

inline void legion::remove_unit(unit* member) {
  auto it = std::find_if(units.begin(), units.end(), [member](unit* child) {
    return (member == child);
  });
  if (it != units.end()) {
    std::swap(units.back(), *it);
    units.pop_back();
  }
}

inline legion::~legion() {
  for (unit* u : units) {
    u->legion_ptr = nullptr;
  }
  //fprintf(stderr, "Legion Destructor\n");
}

inline legion& team::create_legion() {
  legions.push_back(std::make_unique<legion>());
  return (*legions.back());
}

inline team::~team() {
  //fprintf(stderr, "Team Destructor\n");
}

inline std::vector<team*> const& team::get_enemies() {
  return enemy_teams;
}

inline void team::establish_hostility(team* enemy) {
  assert(enemy != nullptr);
  push_unique(enemy_teams, enemy);
  push_unique(enemy->enemy_teams, this);

}

inline bool team::is_hositle(team* enemy) {
  return (std::find(enemy_teams.cbegin(), enemy_teams.cend(), enemy) != enemy_teams.cend());
}

inline bool team::update() {
  // pre-calculate things for the team
  // worry about this if and when it matters
  super::update();
  return false;
}