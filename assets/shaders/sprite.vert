#version 330
layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 tex;


uniform mat3 trans_mat;
uniform mat3 proj_mat;

out vec2 v_tex;

void main() {
  vec3 vPos = proj_mat * trans_mat * vec3(vert, 1.0);
  gl_Position = vec4(vPos.x, vPos.y, 0, 1);
  v_tex = tex;
}
