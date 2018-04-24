#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 projViewMat;
uniform mat4 modelMat;
uniform mat3 normMat;


out vec3 vColor;
out vec3 vNorm;
out vec3 vLightDis;

void main() {
  // eventually make these shader uniform values
  vec3 lightPos = vec3(0.0, 0.0, 0.0);

  vec4 vPos = modelMat * vec4(position, 1.0);
  vLightDis = lightPos -  vec3(vPos);
  gl_Position = projViewMat * vPos;
  vColor = color;
  vNorm = normalize(normMat * normal);
}
