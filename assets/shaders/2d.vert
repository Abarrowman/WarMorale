#version 330
layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 tex;


uniform mat3 transMat;

out vec2 vTex;

void main() {
  vec3 vPos = transMat * vec3(vert, 1.0);
  gl_Position = vec4(vPos.x, vPos.y, 0, vPos.z);
  vTex = tex;
}
