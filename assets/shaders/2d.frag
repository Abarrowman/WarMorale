#version 330

in vec2 vTex;

uniform sampler2D texUnit;

out vec4 fColor;

void main() {
  //fColor = vec4(vTex.x, 0.0, vTex.y, 1.0);
  fColor = texture(texUnit, vTex);
}
