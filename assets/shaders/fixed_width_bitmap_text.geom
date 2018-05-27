#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vertex_data {
  flat int v_char;
} vertex_out[1];

out geom_data {
  vec2 tex_cord;
} geom_out;

uniform mat3 trans_mat;
uniform mat3 proj_mat;
uniform int char_width;
uniform int char_height;
uniform int texture_width;
uniform int texture_height;

vec4 apply_projection(vec2 input) {
  vec3 homo_vec = proj_mat * trans_mat * vec3(input, 1.0);
  return vec4(homo_vec.xy, 0, 1);
}

vec2 char_texture_corner(int character) {
    int idx = character - 32;
    int row_len = texture_width / char_width;
    int row = idx / row_len;
    int col = idx % row_len;
    return vec2((float(col) * char_width) / texture_width, (float(row) * char_height) / texture_height);
  }

void main() {
  vec2 tl_texture_corner = char_texture_corner(vertex_out[0].v_char);

  vec2 top_left = gl_in[0].gl_Position.xy;
  gl_Position = apply_projection(top_left);
  geom_out.tex_cord = tl_texture_corner;
  EmitVertex();
  
  gl_Position = apply_projection(vec2(top_left.x + char_width, top_left.y));
  geom_out.tex_cord = vec2(tl_texture_corner.x + float(char_width) / texture_width, tl_texture_corner.y);
  EmitVertex();
  
  gl_Position = apply_projection(vec2(top_left.x, top_left.y - char_height));
  geom_out.tex_cord = vec2(tl_texture_corner.x, tl_texture_corner.y + float(char_height) / texture_height);
  EmitVertex();
  
  gl_Position = apply_projection(vec2(top_left.x + char_width, top_left.y - char_height));
  geom_out.tex_cord = vec2(tl_texture_corner.x + float(char_width) / texture_width, tl_texture_corner.y + float(char_height) / texture_height);
  EmitVertex();
}