#version 500 core

layout (location = 0) out vec4 vFragColor;

smooth in vec2 vUV;

uniform sampler2D textureMap;
// uniform int uiWidth;
// uniform int uiHeight;

void main() {
  vFragColor = texture(textureMap, vUV/* * vec2(1.0f/uiWidth, 1.0f/uiHeight*/);
}
