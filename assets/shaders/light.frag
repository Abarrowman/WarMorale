#version 330
in vec3 vColor;
in vec3 vNorm;
in vec3 vLightDis;

out vec4 fColor;

void main() {
  // eventually make these shader uniform values
  float diffuseBrightness = 0.5;
  float ambientBrightness = 0.5;
  float specularBrightness = 0.5;
  vec3 cameraDir = vec3(0.0, 0.0, 1.0);
  vec3 specularColor = vec3(1.0, 1.0, 1.0);

  vec3 norm = normalize(vNorm);
  vec3 lightNorm = normalize(vLightDis);
  float diffuse = diffuseBrightness * clamp(dot(norm, lightNorm), 0.0, 1.0);

  vec3 reflected = reflect(-lightNorm, norm);
  float specular = clamp(dot(cameraDir, reflected), 0.0, 1.0);

  vec3 rawColor = vColor * (ambientBrightness + diffuse) + specular * specularBrightness * specularColor;
  fColor = vec4(rawColor, 1.0);
}
