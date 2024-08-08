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
  // Do not support resizing the window
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  const auto window =
      glfwCreateWindow(500, 500, "Video Stabilizer", nullptr, nullptr);
  if (!window) {
    std::cerr << "Error: Could not create GLFW window" << '\n';
    abort();
  }

  glfwSetWindowPos(window, 100, 100);
  glfwMakeContextCurrent(window);

  //------------------------------------------------------ Initialize Glad --//
  if (!gladLoaderLoadGL()) throw std::runtime_error("Error initializing glad");

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(app::debug_cb, nullptr);

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

  //------------------------------------------------------ Compile Shaders --//
  app::shader_builder shader_builder;
  shader_builder.set_shader(GL_VERTEX_SHADER,
                            std::string("//shaders//vertex.glsl"));
  shader_builder.set_shader(GL_FRAGMENT_SHADER,
                            std::string("//shaders//fragment.glsl"));
  const GLuint shader = shader_builder.build();
  glUseProgram(shader);

  //------------------------------------------------------------ Main Loop --//

  glClearColor(0.5, 0.5, 0.5, 1);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // TODO: move below code into dedicated render() function

    //---------------------------------------------------- Start Rendering --//
    // Set up the window flags
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoTitleBar;

    // Set up the window to take up the entire screen
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    //----------------------------------------------------------- Menu Bar --//
    ImGui::Begin("Example", nullptr, window_flags);
    {
      // Remember that we use if-statements here because we
      // should only call EndMenu(Bar) if BeginMenu(Bar) returns true
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Open", "Ctrl+O")) {
            std::cout << "Open file selection dialog\n";
          }
          if (ImGui::MenuItem("Save", "Ctrl+S")) {
            std::cout << "Save current video project\n";
          }
          if (ImGui::MenuItem("Close", "Ctrl+W")) {
            std::cout << "Exiting...\n";
            break;
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
          if (ImGui::MenuItem("View Help", "Ctrl+H")) {
            std::cout << "Open help dialog\n";
            std::cout << "Description of how to use program...\n\n";
          }
          if (ImGui::MenuItem("About", "Ctrl+A")) {
            std::cout << "Open about dialog\n";
            std::cout << "Display version number\n\n";
            std::cout << "Link to GitHub repo...\n\n";
          }
          if (ImGui::MenuItem("Release Notes", "Ctrl+N")) {
            std::cout << "Dialog with Latest release notes...\n\n";
          }
          if (ImGui::MenuItem("Report an Issue", "Ctrl+R")) {
            std::cout << "Link to Bug Report template...\n\n";
          }
          if (ImGui::MenuItem("License", "Ctrl+L")) {
            std::cout << "Open license dialog\n\n";
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      //--------------------------------------------------- Window Content --//
      ImGui::Text("Video Stabilizer");

    }
    ImGui::End();

    //------------------------------------------------------ End Rendering --//

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
