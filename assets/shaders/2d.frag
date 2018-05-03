#version 330

in vec2 v_tex;

uniform sampler2D tex_unit;

uniform int frame_width;
uniform int frame_height;
uniform int frame_col;
uniform int frame_row;

out vec4 f_color;

void main() {

  vec2 norm_tex = vec2(v_tex.x / frame_width, v_tex.y / frame_height);
  vec2 offset_tex = vec2(frame_col / frame_width, frame_row / frame_height);
  vec2 target_tex = norm_tex + offset_tex;

  vec4 tex_value = texture(tex_unit, target_tex);
  f_color = tex_value;
}
