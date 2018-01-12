#include "lib/ui/graphics.hxx"

namespace the::ui {

int Graphics::windowWidth_;
int Graphics::windowHeight_;

Fallible<> Graphics::LoadShaders() {
  auto vertexShader   = LoadFile("lib/shaders/vertex.glsl");
  auto fragmentShader = LoadFile("lib/shaders/fragment.glsl");

  if (auto rv = starsPipeline_.shader.CompileVertex(vertexShader.c_str()); !rv)
    return rv;
  if (auto rv = starsPipeline_.shader.CompileFragment(fragmentShader.c_str()); !rv)
    return rv;
  if (auto rv = starsPipeline_.shader.LinkProgramme(); !rv)
    return rv;

  starsPipeline_.programme = starsPipeline_.shader.Programme();
  starsPipeline_.modelToWorldMatrix = glGetUniformLocation(starsPipeline_.programme, "modelToWorldMatrix");

  return {};
}

}
