#pragma once

#include "renderable.h"

class unit;

class command {
public:
  vector_2f pos{ 0, 0 };
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

class team : public renderable_parent<true, unit> {
private:
  using super = renderable_parent<true, unit>;
  std::vector<std::unique_ptr<legion>> legions;
  std::vector<team*> enemy_teams;
public:

  std::vector<team*> const& get_enemies();

  void establish_hostility(team* enemy);

  bool update() override;

  legion& create_legion();

  ~team();
};