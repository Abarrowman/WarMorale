#version 330

in vec2 v_tex;

uniform sampler2D tex_unit;

uniform ivec2 frames;
uniform ivec2 current_frame;

uniform vec4 mask_color;

out vec4 f_color;

void main() {

  vec2 norm_tex = v_tex / frames;
  vec2 offset_tex = vec2(current_frame) / frames;
  vec2 target_tex = norm_tex + offset_tex;

  vec4 tex_value = texture(tex_unit, target_tex); 
  f_color = tex_value * mask_color;
}
