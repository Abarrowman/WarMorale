#pragma once
#include "utils.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include "fixed_width_bitmap_font.h"
#include "variable_width_bitmap_font.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

enum class static_texture_id {
  face,
  fire,
  grunt,
  ceres,
  COUNT
};

enum class static_shader_id {
  sprite,
  polygon_fill,
  line,
  point_particle,
  fixed_width_bitmap_text,
  variable_width_bitmap_text,
  COUNT
};

enum class static_vertex_array_id {
  sprite,
  dodecagon, // 12-sidded regular polygon basically a circle
  COUNT
};

enum class static_mono_font_id {
  consolas_12,
  COUNT
};

enum class static_prop_font_id {
  impact_24,
  COUNT
};


class static_resources {
private:
  std::vector<texture> textures;
  std::vector<shader> shaders;
  std::vector<simple_vertex_array> vertex_arrays;
  std::vector<fixed_width_bitmap_font> mono_fonts;
  std::vector<variable_width_bitmap_font> prop_fonts;

public:
  static_resources() {
    // pre-allocate to avoid pointers to avoid reference invalidation
    textures.reserve(static_cast<int>(static_texture_id::COUNT));
    shaders.reserve(static_cast<int>(static_shader_id::COUNT));
    vertex_arrays.reserve(static_cast<int>(static_vertex_array_id::COUNT));
    mono_fonts.reserve(static_cast<int>(static_mono_font_id::COUNT));
    prop_fonts.reserve(static_cast<int>(static_prop_font_id::COUNT));

    textures.emplace_back("./assets/textures/sad.png");
    textures.emplace_back("./assets/textures/fire.png");
    textures.emplace_back("./assets/textures/grunt.png");
    textures.emplace_back("./assets/textures/ceres.png");

    {
      std::string const sprite_vertex_shader = read_file_to_string("./assets/shaders/sprite.vert");
      std::string const sprite_fragment_shader = read_file_to_string("./assets/shaders/sprite.frag");
      shaders.emplace_back(sprite_vertex_shader.c_str(), sprite_fragment_shader.c_str());
    }
    {
      std::string const polygon_vertex_shader = read_file_to_string("./assets/shaders/polygon.vert");
      std::string const solid_fragment_shader = read_file_to_string("./assets/shaders/solid.frag");
      shaders.emplace_back(polygon_vertex_shader.c_str(), solid_fragment_shader.c_str());


      std::string const line_vertex_shader = read_file_to_string("./assets/shaders/line.vert");
      std::string const line_geometry_shader = read_file_to_string("./assets/shaders/line.geom");
      // TODO multiple copies of the same shaders are being compiled into programs
      shaders.emplace_back(line_vertex_shader.c_str(), line_geometry_shader.c_str(), solid_fragment_shader.c_str());
    }
    {
      std::string const pt_particle_vertex_shader = read_file_to_string("./assets/shaders/pt_particle.vert");
      std::string const vertex_interp_fragment_shader = read_file_to_string("./assets/shaders/vertex_interp.frag");
      shaders.emplace_back(pt_particle_vertex_shader.c_str(), vertex_interp_fragment_shader.c_str());
    }
    {
      std::string const mono_vertex_shader = read_file_to_string("./assets/shaders/fixed_width_bitmap_text.vert");
      std::string const mono_geometry_shader = read_file_to_string("./assets/shaders/fixed_width_bitmap_text.geom");
      std::string const fragment_shader = read_file_to_string("./assets/shaders/bitmap_text.frag");
      shaders.emplace_back(mono_vertex_shader.c_str(), mono_geometry_shader.c_str(), fragment_shader.c_str());

      std::string const prop_vertex_shader = read_file_to_string("./assets/shaders/variable_width_bitmap_text.vert");
      std::string const prop_geometry_shader = read_file_to_string("./assets/shaders/variable_width_bitmap_text.geom");
      shaders.emplace_back(prop_vertex_shader.c_str(), prop_geometry_shader.c_str(), fragment_shader.c_str());
    }
    
    {
      vertex_arrays.push_back(simple_vertex_array::create_sprite_vertex_array());
    }
    {
      vertex_arrays.push_back(simple_vertex_array::create_circle<12>());
    }

    {
      texture tex{ "./assets/fonts/consolas_12.png" };
      mono_fonts.emplace_back(std::move(tex), 8, 16);
    }

    {
      texture tex{ "./assets/fonts/impact_24.png" };
      prop_fonts.emplace_back(std::move(tex), read_csv_no_header("./assets/fonts/impact_24.csv"));
    }

    assert(textures.capacity() == textures.size());
    assert(shaders.capacity() == shaders.size());
    assert(vertex_arrays.capacity() == vertex_arrays.size());
    assert(mono_fonts.capacity() == mono_fonts.size());
    assert(prop_fonts.capacity() == prop_fonts.size());

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
  simple_vertex_array& get_vertex_array(static_vertex_array_id id) {
    return vertex_arrays[static_cast<int>(id)];
  }

  /*
  Return the statically mapped shader associated with the id.
  O(1)
  */
  shader& get_shader(static_shader_id id) {
    return shaders[static_cast<int>(id)];
  }


  /*
  Return the statically mapped mono-spaced font associated with the id.
  O(1)
  */
  fixed_width_bitmap_font& get_mono_font(static_mono_font_id id) {
    return mono_fonts[static_cast<int>(id)];
  }

  /*
  Return the statically  mapped proportionally-spaced font associated with the id.
  O(1)
  */
  variable_width_bitmap_font& get_prop_font(static_prop_font_id id) {
    return prop_fonts[static_cast<int>(id)];
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
  std::unordered_map<K, std::unique_ptr<simple_vertex_array>> vertex_arrays;
public:

  bool has_texture(K const& id) {
    return (textures.end() != textures.find(id));

  }

  bool has_shader(K const& id) {
    return (shaders.end() != shaders.find(id));

  }

  bool has_vertex_array(K const& id) {
    return (simple_vertex_array.end() != vertex_array.find(id));

  }

  texture& get_texture(K const& id) {
    return *(textures[id].get());
  }

  shader& get_shader(K const& id) {
    return *(textures[id].get());
  }

  simple_vertex_array& get_vertex_array(K const& id) {
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