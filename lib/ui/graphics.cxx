#include <thread>

#include "lib/ui/graphics.hxx"
#include "lib/ui/errors.hxx"

namespace the::ui {

int Graphics::windowWidth_;
int Graphics::windowHeight_;

Fallible<> Graphics::Init() {
  using namespace std::placeholders;

  glfwSetErrorCallback(Graphics::OnGlfwErrorCallback_);

  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    return {OglRuntimeError{"could not start GLFW3"}};
  }
  glfwInitialized_ = true;

#ifdef __APPLE__
  // Get the newest available version of OpenGL on Apple,
  // which will be 4.1 or 3.3 on Mavericks, and 3.2 on
  // pre-Mavericks systems.
  // On other systems it will tend to pick 3.2 instead of
  // the newest version, which is unhelpful.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
  glfwWindowHint(GLFW_SAMPLES, 4);

  // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  // GLFWvidmode const *vmode = glfwGetVideoMode(monitor);
  // window_ = glfwCreateWindow(
  //     vmode->width, vmode->height, "Stars sky", monitor, NULL);
  window_ = glfwCreateWindow(
      WindowWidth(), WindowHeight(), "Stars sky", nullptr, nullptr);
  if (!window_) {
    return {OglRuntimeError{"could not open window with GLFW3"}};
  }
  glfwGetFramebufferSize(window_, &Graphics::windowWidth_, &Graphics::windowHeight_);
  FALL_ON_GL_ERROR();
  glfwSetWindowSizeCallback(window_, &Graphics::OnGlfwWindowSizeCallback_);
  FALL_ON_GL_ERROR();
  glfwMakeContextCurrent(window_);
  FALL_ON_GL_ERROR();

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();
  FALL_ON_GL_ERROR();

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  FALL_ON_GL_ERROR();
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  FALL_ON_GL_ERROR();
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
  FALL_ON_GL_ERROR();

  return {};
}

Fallible<> Graphics::Deinit() {
  // close GL context and any other GLFW resources
  if (!glfwInitialized_) {
    glfwTerminate();
    glfwInitialized_ = false;
  }
  // TODO: Delete only if it has been compiled before.
  glDeleteProgram(starsPipeline_.programme);
  glDeleteVertexArrays(1, &starsPipeline_.vao);
  FALL_ON_GL_ERROR();

  return {};
}

Fallible<> Graphics::LoadShaders() {
  auto vertexShader   = LoadFile("lib/shaders/vertex.glsl");
  auto fragmentShader = LoadFile("lib/shaders/fragment.glsl");

  if (auto rv = starsPipeline_.shader.CompileVertex(vertexShader); !rv)
    return rv;
  if (auto rv = starsPipeline_.shader.CompileFragment(fragmentShader); !rv)
    return rv;
  if (auto rv = starsPipeline_.shader.LinkProgramme(); !rv)
    return rv;

  starsPipeline_.programme = starsPipeline_.shader.Programme();
  starsPipeline_.modelToWorldMatrix = glGetUniformLocation(starsPipeline_.programme, "modelToWorldMatrix");

  return {};
}

Fallible<> Graphics::Loop() {
  using namespace std::literals::chrono_literals;

  static constexpr auto perfectFps = 60u;
  static constexpr auto frameTimeslice = 1s / perfectFps;

  while (!glfwWindowShouldClose(window_)) {
    auto const renderingAt = std::chrono::steady_clock::now();

    if (auto rv = HandleInput(); !rv) {
      ERROR() << "failed to handle the input: " << rv.Err();
    }

    glViewport(0, 0, windowWidth_, windowHeight_);
    FALL_ON_GL_ERROR();

    // Wipe the drawing surface clear.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    FALL_ON_GL_ERROR();

    if (auto rv = Render(); !rv) {
      ERROR() << "failed to render a frame: " << rv.Err();
    }

    // Update other events like input handling.
    glfwPollEvents();
    FALL_ON_GL_ERROR();

    // Put the stuff we've been drawing onto the display.
    glfwSwapBuffers(window_);
    FALL_ON_GL_ERROR();

    auto const finishedRenderingAt = std::chrono::steady_clock::now();
    auto const renderingTook = finishedRenderingAt - renderingAt;
    lastFrameTook_ = renderingTook;

    if (renderingTook < frameTimeslice) {
      std::this_thread::sleep_for(frameTimeslice - renderingTook);
    }
  }

  return {};
}

void Graphics::OnGlfwWindowSizeCallback_(GLFWwindow *window, int width, int height) {
  glfwGetFramebufferSize(window, &width, &height);
  // auto const dim = std::min(width, height);
  windowWidth_ = width;
  windowHeight_ = height;
  // TODO: update any perspective matrices used here
}

void Graphics::OnGlfwErrorCallback_(int error, char const *description) {
  ERROR() << "GLFW: code=" << error << ": " << description;
}

}
