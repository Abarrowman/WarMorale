#pragma once
#include "utils.h"
#include <vector>

enum class static_texture_id {
  face,
  fire,
  ship,
  COUNT
};

enum class static_shader_id {
  sprite,
  COUNT
};

enum class static_vertex_array_id {
  sprite,
  COUNT
};


class static_resources {
private:
  std::vector<texture> textures;
  std::vector<shader> shaders;
  std::vector<vertex_array> vertex_arrays;
public:
  static_resources() {
    // pre-allocate to avoid pointers to avoid pointer invalidation
    textures.reserve(static_cast<int>(static_texture_id::COUNT));
    shaders.reserve(static_cast<int>(static_shader_id::COUNT));
    vertex_arrays.reserve(static_cast<int>(static_vertex_array_id::COUNT));

    textures.emplace_back("./assets/textures/sad.png");
    textures.emplace_back("./assets/textures/fire.png");
    textures.emplace_back("./assets/textures/ship.png");


    std::string const sprite_fragment_shader = read_file_to_string("./assets/shaders/2d.frag");
    std::string const sprite_vertex_shader = read_file_to_string("./assets/shaders/2d.vert");
    shaders.emplace_back(sprite_vertex_shader.c_str(), sprite_fragment_shader.c_str());


    vertex_array sprite_vertex_array = vertex_array::create_sprite_vertex_array();
    vertex_arrays.push_back(std::move(sprite_vertex_array));
  }

  /*
  Return the statically mapped texture associated with the id.
  O(1)
  */
  texture& get_texture(static_texture_id id) {
    return textures[static_cast<int>(id)];
  }

  /*
  Return the statically mapped vertex array associated with the id.
  O(1)
  */
  vertex_array& get_vertex_array(static_vertex_array_id id) {
    return vertex_arrays[static_cast<int>(id)];
  }

  /*
  Return the statically mapped shader associated with the id.
  O(1)
  */
  shader& get_shader(static_shader_id id) {
    return shaders[static_cast<int>(id)];
  }


  // do not copy, assign, or move
  static_resources(static_resources&) = delete;
  static_resources& operator=(const static_resources&) = delete;
  static_resources(static_resources&& old) = delete;
};