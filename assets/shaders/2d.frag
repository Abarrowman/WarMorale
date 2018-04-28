#version 330

in vec2 v_tex;

uniform sampler2D tex_unit;

out vec4 fColor;

void main() {
  //fColor = vec4(v_tex.x, 0.0, v_tex.y, 1.0);

  vec4 tex_value = texture(tex_unit, v_tex);
  fColor = tex_value;
  //fColor = vec4(tex_value.w, 0, 0, 1);
}
