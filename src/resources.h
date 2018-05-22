#pragma once
#include "utils.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

enum class static_texture_id {
  face,
  fire,
  grunt,
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
    // pre-allocate to avoid pointers to avoid reference invalidation
    textures.reserve(static_cast<int>(static_texture_id::COUNT));
    shaders.reserve(static_cast<int>(static_shader_id::COUNT));
    vertex_arrays.reserve(static_cast<int>(static_vertex_array_id::COUNT));

    textures.emplace_back("./assets/textures/sad.png");
    textures.emplace_back("./assets/textures/fire.png");
    textures.emplace_back("./assets/textures/grunt.png");

    std::string const sprite_fragment_shader = read_file_to_string("./assets/shaders/sprite.frag");
    std::string const sprite_vertex_shader = read_file_to_string("./assets/shaders/sprite.vert");
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


  // do not copy or assign
  static_resources(static_resources&) = delete;
  static_resources& operator=(const static_resources&) = delete;
};

/*
Maps to unique pointers are used to allow insertion and deletion without reference invalidation
*/
template<typename K>
class dyanamic_resources {
private:
  std::unordered_map<K, std::unique_ptr<texture>> textures;
  std::unordered_map<K, std::unique_ptr<shader>> shaders;
  std::unordered_map<K, std::unique_ptr<vertex_array>> vertex_arrays;
public:

  bool has_texture(K const& id) {
    return (textures.end() != textures.find(id));

  }

  bool has_shader(K const& id) {
    return (shaders.end() != shaders.find(id));

  }

  bool has_vertex_array(K const& id) {
    return (vertex_array.end() != vertex_array.find(id));

  }

  texture& get_texture(K const& id) {
    return *(textures[id].get());
  }

  shader& get_shader(K const& id) {
    return *(textures[id].get());
  }

  vertex_array& get_vertex_array(K const& id) {
    return *(vertex_arrays[id].get());
  }

  void add_texutre(K const& id, std::unique_ptr<texture> texture) {
    textures[id] = std::move(texture);
  }

  dyanamic_resources() {}

  // do not copy or assign
  dyanamic_resources(dyanamic_resources&) = delete;
  dyanamic_resources& operator=(const dyanamic_resources&) = delete;
};

using int_keyed_resources = dyanamic_resources<int>;