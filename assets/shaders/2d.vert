#version 330
layout(location = 0) in vec2 position;

uniform mat3 transMat;

void main() {
  vec3 vPos = transMat * vec3(position, 1.0);
  gl_Position = vec4(vPos.x, vPos.y, 0, vPos.z);
}
