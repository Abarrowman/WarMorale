#pragma once

#include "2d_math.h"

class point_threat;
class team;
class legion;
class world;

struct unit_archetype {
  int const max_health;
  float const potential_radius;
  float const max_speed;
};

enum unit_status {
  LIVING,
  KILLED,
  DYING
};

class unit_reference;

class unit {
private:
  unit_status status = unit_status::LIVING;
  vector_2f old_pos;

  std::vector<unit_reference> references;

  void take_threats();
public:
  world& world_ref;
  team& team_ref;
  unit_archetype const& type;
  legion* legion_ptr;

  trans_state trans;
  bool visible = true;

  // refactor into class maybe
  int current_health;


  unit(world& w, team& t, legion* l, unit_archetype const& ty);
  virtual ~unit(); // base class
  bool update();
  bool is_living();

  virtual void render(matrix_3f const& parent_trans) = 0;
  virtual bool take_point_threat(point_threat& pt);

  unit_reference ref();
protected:
  /*
  Updates the unit.
  */
  virtual void living_update();
  virtual void death_action() {}

  // Find the closest enemy of all enemies
  unit_reference find_closest_enemy();

  // Find the closest enemy within a vector of unit references
  unit_reference find_closest_enemy(std::vector<unit_reference>& references);
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
  bool operator==(unit_reference const& other) const;
};

template<typename T>
inline T* create_unit(world& w, team& t, legion* l) {
  return t.add_orphan(new T(w, t, l));
}