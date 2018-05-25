#pragma once

#include "matrix_3f.h"

class point_threat;
class team;
class legion;
class world;

enum unit_status {
  LIVING,
  KILLED,
  DYING
};

class unit_reference;

class unit : public ordered_parent {
private:
  unit_status status = unit_status::LIVING;
  void take_threats();
public:
  world& land;
  team& side;
  legion* group;
  int const max_health;
  int current_health;

  trans_state trans;

  unit(world& w, team& t, legion* l, int max_hp);
  virtual ~unit(); // base class
  bool update() override;
  bool is_living();

  virtual bool take_point_threat(point_threat& pt) = 0;


protected:
  /*
  Updates the unit.
  */
  virtual void living_update() = 0;
  virtual void death_action() {}

  unit_reference find_closest_enemy();
};

/*
A class to represent a reference to a unit that may be null, or now refer to a dead unit.
*/
class unit_reference {
private:
  unit* target;

public:
  unit_reference() : target(nullptr) {}
  unit_reference(unit& t) : target(&t) {}
  unit_reference(unit* t) : target(t) {}

  void update();
  bool valid();
  unit* ptr();
  unit& ref();
};