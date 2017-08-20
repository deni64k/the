#version 400 core

// layout (location = 0) in VS_IN {
//   vec3 vp;
//   float mag;
// }

in vec4 vp;
// in float mag;
// layout (location = 1) in vec4 color;

// out VS_OUT {
//   vec4 color;
// } vs_out;

// Returns an appropriate size based on apparent magnitude. Not scientifically correct.
float starSize(in float m, in float size, in float modifier) {
  return size * pow(modifier, 1.0 - m);
}

// Returns an appropriate brightness based on apparent magnitude. Not scientifically correct.
float starBrightness(in float m, in float lower, in float upper) {
  return pow(clamp((lower + (1.0 - m)) / (upper + lower), 0.0, 1.0), 1.5);
}

void main() {
  gl_Position = vec4(vp.x, vp.y, vp.z, 1.0);
  gl_PointSize = vp.w;//mag;
  // vs_out.color = color;
}
