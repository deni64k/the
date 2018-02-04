#version 400 core

// in VS_OUT {
//   vec4 color;
// } fs_in;

out vec4 color;

void main() {
  // color = vec4(sin(gl_FragCoord.x * 0.25) * 0.5 * fs_in.color.x + 0.5,
  //              cos(gl_FragCoord.y * 0.25) * 0.5 * fs_in.color.y + 0.5,
  //              sin(gl_FragCoord.x * 0.15) * cos(gl_FragCoord.y * 0.15),
  //              1.0);
  // color = fs_in.color;
  vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
  if (dot(circCoord, circCoord) > 1.0) {
    discard;
  }

  color = vec4(1.0, 1.0, 1.0, 1.0);
}
