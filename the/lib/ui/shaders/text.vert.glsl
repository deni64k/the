#version 330 core

layout (location = 0) in vec2 vVertex;

smooth out vec2 vUV;

void main() {
  vec2 pos = vVertex;
  pos.y *= 0.1;
  pos -= 1;

  gl_Position = vec4(pos, 0.0, 1.0);

  vUV = vVertex;
}
