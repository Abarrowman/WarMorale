#version 330

layout(lines) in;

// square and butt cap only require max_vertices 4
//layout(triangle_strip, max_vertices = 4) out;

// circle cap requires 4 + 2 * (round_segments=5)
layout(triangle_strip, max_vertices = 14) out;

uniform mat3 trans_mat;
uniform float width;

uniform int cap_type;

const int butt_cap = 0;
const int square_cap = 1;
const int round_cap = 2;

const int round_segments = 5;
const int round_segments_floor = round_segments / 2;
const int round_segments_ceil = round_segments - round_segments_floor;

const float PI_F = 3.1415926535897932384626433832795;

vec4 apply_projection(vec2 input) {
  vec3 homo_vec = trans_mat * vec3(input, 1.0);
  return vec4(homo_vec.xy, 0, 1);
}

vec4 calculate_end_point(int i, float half_width, vec2 start, vec2 diff_norm , vec2 perp_norm) {
  float ang = (i + 1) * PI_F / (round_segments + 1);
  vec2 round_seg = start + half_width * (cos(ang) * -perp_norm + sin(ang) * -diff_norm);
  return apply_projection(round_seg); 
}

void main() {
  vec2 start = gl_in[0].gl_Position.xy;
  vec2 end = gl_in[1].gl_Position.xy;
	vec2 diff = end - start;
	vec2 perp = vec2(-diff.y, diff.x);
	vec2 diff_norm = normalize(diff);
	vec2 perp_norm = normalize(perp);
  
  float half_width = width * 0.5;

  vec2 top_left;
  vec2 bottom_left;
  vec2 top_right;
  vec2 bottom_right;
  if ((cap_type == butt_cap) || (cap_type == round_cap)) {
    // butt cap
    top_left = start - half_width * perp_norm;
    bottom_left = start - half_width * -perp_norm;
    top_right = end + half_width * -perp_norm;
    bottom_right = end + half_width * perp_norm;
  } else {
    // square cap
    top_left = start - half_width * (diff_norm + perp_norm);
    bottom_left = start - half_width * (diff_norm - perp_norm);
    top_right = end + half_width * (diff_norm - perp_norm);
    bottom_right = end + half_width * (diff_norm + perp_norm);
  }
	
  vec4 top_left_proj = apply_projection(top_left);
  vec4 bottom_left_proj = apply_projection(bottom_left);
  vec4 top_right_proj = apply_projection(top_right);
  vec4 bottom_right_proj = apply_projection(bottom_right);
  
  if (cap_type == round_cap) {
    if (round_segments_ceil != round_segments_floor) {
      gl_Position = calculate_end_point(round_segments_floor, half_width, start, diff_norm, perp_norm);
      EmitVertex();
    }
    for (int i = 0; i < round_segments_floor; i++) {
      gl_Position = calculate_end_point(round_segments_floor - 1 - i, half_width, start, diff_norm, perp_norm);
      EmitVertex();
      gl_Position = calculate_end_point(round_segments_ceil + i, half_width, start, diff_norm, perp_norm);
      EmitVertex();
    }
  }
  
  gl_Position = top_left_proj;
  EmitVertex();
  gl_Position = bottom_left_proj;
  EmitVertex();
  gl_Position = top_right_proj;
  EmitVertex();
  gl_Position = bottom_right_proj;
  EmitVertex();
  
  if (cap_type == round_cap) {
    for (int i = 0; i < round_segments_floor; i++) {
      gl_Position = calculate_end_point(i, half_width, end, -diff_norm, perp_norm);
      EmitVertex();
      gl_Position = calculate_end_point(round_segments - 1 - i, half_width, end, -diff_norm, perp_norm);
      EmitVertex();
    }
    if (round_segments_ceil != round_segments_floor) {
      gl_Position = calculate_end_point(round_segments_floor, half_width, end, -diff_norm, perp_norm);
      EmitVertex();
    }
  }
}