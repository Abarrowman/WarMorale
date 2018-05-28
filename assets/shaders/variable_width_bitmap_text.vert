#version 330
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex_cord;
layout(location = 2) in float width;

out vertex_data {
  vec2 tex_cord;
  float width;
} vertex_out;

void main() {
  gl_Position = vec4(pos, 0, 1);
  vertex_out.tex_cord = tex_cord;
  vertex_out.width = width;
}
