#version 330

layout(location = 0) in vec2 vert;

uniform mat3 trans_mat;

void main() {
  vec3 vPos = trans_mat * vec3(vert, 1.0);
  gl_Position = vec4(vPos.x, vPos.y, 0, 1);
}