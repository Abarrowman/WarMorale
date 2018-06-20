#pragma once

#include "renderable.h"
#include "color.h"
#include "geom.h"

class unit;

class command {
public:
  vector_2f pos{ 0, 0 };
  precalc_polygon formation;

  vector_2f get_potential_force(vector_2f location);
};

class legion {
private:
  std::vector<unit*> units;
public:
  command order;

  void add_unit(unit* member);

  void remove_unit(unit* member);

  ~legion();
};

class team : public renderable_parent<unit, true> {
private:
  using super = renderable_parent<unit, true>;
  std::vector<std::unique_ptr<legion>> legions;
  std::vector<team*> enemy_teams;

public:
  color const col;

  team(color c) : col(c) {};

  std::vector<team*> const& get_enemies();

  void establish_hostility(team* enemy);

  bool is_hositle(team* enemy);

  bool update() override;

  legion& create_legion();

  ~team();
};

using team_parent = renderable_parent<team, true>;