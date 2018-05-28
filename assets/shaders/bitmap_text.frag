#version 330

in geom_data {
  vec2 tex_cord;
} geom_out;

uniform sampler2D tex_unit;
uniform vec4 color;

out vec4 f_color;

void main() {
  float font_alpha = texture(tex_unit, geom_out.tex_cord).r;
  f_color = vec4(color.rgb, color.a * font_alpha);
}