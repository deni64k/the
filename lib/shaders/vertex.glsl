#version 400 core

uniform mat4 modelToWorldMatrix = mat4(1.0);

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
  // float fov = (60.0f / 2.0f) * 3.14f / 180.0f;
  // float tanFov = tan(fov);
  // float zNear = 0.1f;
  // float zFar = 1000.0f;
  // float zRange = zNear - zFar;
  // float ratio = 1.0f;

  // mat4 proj = mat4(
  //     vec4(1.0f / (tanFov * ratio), 0.0f,          0.0f,                     0.0f),
  //     vec4(0.0f,                    1.0f / tanFov, 0.0f,                     0.0f),
  //     vec4(0.0f,                    0.0f,          (-zNear - zFar) / zRange, 2.0f * zFar * zNear / zRange),
  //     vec4(0.0f,                    0.0f,          1.0f,                     0.0f)
  // );
             
  // mat4 cam = mat4(
  //     vec4(1.0f, 0.0f, 0.0f, 0.0f),
  //     vec4(0.0f, 1.0f, 0.0f, 0.0f),
  //     vec4(0.0f, 0.0f, 1.0f, 0.0f),
  //     vec4(0.0f, 0.0f, 0.0f, 1.0f)
  // );
  // mat4 trans = mat4(
  //     vec4(1.0f, 0.0f, 0.0f, -0.0f),
  //     vec4(0.0f, 1.0f, 0.0f, -0.0f),
  //     vec4(0.0f, 0.0f, 1.0f, -0.0f),
  //     vec4(0.0f, 0.0f, 0.0f,  1.0f)
  // );
  // mat4 view = cam * trans;

  gl_Position = modelToWorldMatrix * vec4(vp.x, vp.y, vp.z, 1.0);
  gl_PointSize = vp.w;//mag;
  // vs_out.color = color;
}
