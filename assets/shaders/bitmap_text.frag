#version 330

in vec2 v_tex;

uniform sampler2D tex_unit;
uniform vec4 color;

out vec4 f_color;

void main() {
  float font_alpha = texture(tex_unit, v_tex).r;
  f_color = vec4(color.rgb, color.a * font_alpha);
}