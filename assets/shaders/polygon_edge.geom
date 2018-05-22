#version 330

layout(lines) in;

layout(triangle_strip, max_vertices = 4) out;

uniform mat3 trans_mat;
uniform mat3 proj_mat;
uniform float width;

vec4 apply_projection(vec2 input) {
  vec3 homo_vec = proj_mat * trans_mat * vec3(input, 1.0);
  return vec4(homo_vec.xy, 0, 1);
}

void main() {
  vec2 start = gl_in[0].gl_Position.xy;
  vec2 end = gl_in[1].gl_Position.xy;
	vec2 diff = end - start;
	vec2 perp = vec2(-diff.y, diff.x);
	vec2 diff_norm = normalize(diff);
	vec2 perp_norm = normalize(perp);
  
  float half_width = width / 2;

  // square cap
	/*vec2 top_left = start - half_width * (diff_norm + perp_norm);
	vec2 bottom_left = start - half_width * (diff_norm - perp_norm);
	vec2 top_right = end + half_width * (diff_norm - perp_norm);
	vec2 bottom_right = end + half_width * (diff_norm + perp_norm);*/
  
  // butt cap
  vec2 top_left = start - half_width * perp_norm;
	vec2 bottom_left = start - half_width * -perp_norm;
	vec2 top_right = end + half_width * -perp_norm;
	vec2 bottom_right = end + half_width * perp_norm;

	gl_Position = apply_projection(top_left);
	EmitVertex();
	gl_Position = apply_projection(top_right);
	EmitVertex();
	gl_Position = apply_projection(bottom_left);
	EmitVertex();
	gl_Position = apply_projection(bottom_right);
	EmitVertex();
}