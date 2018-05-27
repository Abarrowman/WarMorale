#version 330
layout(location = 0) in vec2 pos;
layout(location = 1) in int character;
out vertex_data {
  flat int v_char;
} vertex_out;

void main() {
  gl_Position = vec4(pos, 0, 1);
  vertex_out.v_char = character;
}
