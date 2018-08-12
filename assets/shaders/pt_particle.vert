#version 330
layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;

uniform mat3 trans_mat;

out vec4 v_color;


void main() {
  vec3 pPos = trans_mat * vec3(pos, 1.0);
  gl_Position = vec4(pPos.x, pPos.y, 0, 1);
  v_color = color;
}
