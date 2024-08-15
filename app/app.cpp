#include "app/app.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

#include "app/shader.h"

// TODO: Create error codes

auto main() -> int {
  //------------------------------------------------------ Initialize GLFW --//
  if (!app::init_glfw()) {
    std::cerr << "Error: Could not initialize GLFW" << '\n';
    abort();
  }

  //-------------------------------------------------------- Create window --//
  const auto window =
      glfwCreateWindow(500, 500, "Video Stabilizer", nullptr, nullptr);
  if (!window) {
    std::cerr << "Error: Could not create GLFW window" << '\n';
    abort();
  }

  glfwMakeContextCurrent(window);

  //------------------------------------------------------ Initialize Glad --//
  if (!gladLoaderLoadGL()) throw std::runtime_error("Error initializing glad");

  //----------------------------------------------------------------- Logs --//
  int glfw_major, glfw_minor, glfw_revision;
  glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);

  // TODO: Add logging support
  std::cout << "Using OpenGL " << glGetString(GL_VERSION) << '\n';
  std::cout << "Using GLAD " << '\n';
  std::cout << "Using GLFW " << glfw_major << "." << glfw_minor << "."
            << glfw_revision << '\n';

  //----------------------------------------------------- Initialize ImGui --//
  app::init_imgui(window);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(app::debug_cb, nullptr);

  //------------------------------------------------------ Compile Shaders --//
  app::shader_builder shader_builder;
  shader_builder.set_shader(
      GL_VERTEX_SHADER, std::string("//shaders//vertex.glsl"));
  shader_builder.set_shader(
      GL_FRAGMENT_SHADER, std::string("//shaders//fragment.glsl"));
  const GLuint shader = shader_builder.build();
  glUseProgram(shader);

  //------------------------------------------------------------ Main Loop --//

  glClearColor(0.5, 0.5, 0.5, 1);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool show_demo = false;
    ImGui::Begin("Example");
    if (ImGui::Button("Show/Hide ImGui demo")) show_demo = !show_demo;
    ImGui::End();
    if (show_demo) ImGui::ShowDemoWindow(&show_demo);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // TODO: select load video from disk
  // TODO: detect(?) and set frame rate
  // TODO: play the original and stabilized video for side-by-side comparison
  // TODO: save video to disk


  app::shutdown(window);

  return EXIT_SUCCESS;
}
