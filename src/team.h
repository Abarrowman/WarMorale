#pragma once

#include "utils.h"
#include "team_face.h"
#include "unit_face.h"

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
    u->group = nullptr;
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

inline bool team::update() {
  // pre-calculate things for the team
  // worry about this if and when it matters
  super::update();
  return false;
}