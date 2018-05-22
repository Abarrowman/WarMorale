#pragma once

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

public:
  world& land;
  team& side;
  legion* group;

  trans_state trans;

  unit(world& w, team& t, legion* l);
  virtual ~unit(); // base class
  bool update() override;
  bool is_living();

protected:
  /*
  Updates the unit.
  Return true if the unit should die.
  */
  virtual bool living_update() { return false; }

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