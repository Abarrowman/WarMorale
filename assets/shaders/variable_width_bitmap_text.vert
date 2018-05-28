#version 330
layout(location = 0) in vec2 pos;
layout(location = 1) in float width;
layout(location = 2) in int character;


out vertex_data {
  flat int v_char;
  float width;
} vertex_out;

void main() {
  gl_Position = vec4(pos, 0, 1);
  vertex_out.v_char = character;
  vertex_out.width = width;
}
