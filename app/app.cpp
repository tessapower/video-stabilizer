/// PROJECT: Video Stabilizer
/// DESCRIPTION: Stabilizes and crops a video.
///
/// AUTHOR: Tessa Power
/// DATE: August 1st, 2024
///
/// LICENSE: MIT License
///
/// Copyright(c) 2024 Tessa Power
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without
/// limitation the rights to use, copy, modify, merge, publish, distribute,
/// sublicense, and/or sell copies of the Software, and to permit persons to
/// whom the Software is furnished to do so, subject to the following
/// conditions:
///
///   The above copyright notice and this permission notice shall be included
///   in all copies or substantial portions of the Software.
///
///   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
///   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
///   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
///   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
///   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
///   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
///   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// USAGE: How to compile/run this file, e.g., "Compile with: g++ main.cpp -o
/// myApp"
///
/// NOTES: This project expects that OpenCV is installed locally on your system
///

#include "app/app.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

#include "app/gui.h"
#include "app/shader.h"

auto main() -> int {
  //----------------------------------------------- Initialize GLFW system --//
  // TODO: Create error codes to handle initialization errors
  if (!app::init_glfw()) {
    std::cerr << "Error: Could not initialize GLFW" << '\n';
    abort();
  }

  //--------------------------------------------------- Create GLFW window --//
  // Do not support resizing the window to keep things simple
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  const auto window = glfwCreateWindow(app::window_width, app::window_height,
                                       "Video Stabilizer", nullptr, nullptr);

  if (!window) {
    std::cerr << "Error: Could not create GLFW window" << '\n';
    abort();
  }

  // TODO: set up the window to open centered on the screen
  glfwSetWindowPos(window, 100, 100);
  // Make our new window the current context for OpenGL
  glfwMakeContextCurrent(window);

  //----------------------------------------------- Initialize GLAD system --//
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    throw std::runtime_error("Error initializing glad");

  // Enable debugging OpenGL and pass it a callback function to use
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(app::debug_cb, nullptr);

  //------------------------------------ Log dependency versions to stdout --//
  int glfw_major, glfw_minor, glfw_revision;
  glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);

  // TODO: Add logging support
  std::cout << "Using OpenGL " << glGetString(GL_VERSION) << '\n';
  std::cout << "Using GLAD " << GLVersion.major << "." << GLVersion.minor << '\n';
  std::cout << "Using GLFW " << glfw_major << "." << glfw_minor << "."
            << glfw_revision << '\n';
  std::cout << "Using ImGui " << IMGUI_VERSION << "\n";

  //----------------------------------------------------- Initialize ImGui --//
  // This handles all the verbose setup code for our ImGui window
  app::init_imgui(window);

  //------------------------------------------------------ Compile Shaders --//
  // Because we are using OpenGL, we need some default shaders, so we pass in
  // the bare minimum to the shader builder.
  app::shader_builder shader_builder;
  shader_builder.set_shader(GL_VERTEX_SHADER,
                            std::string("//shaders//vertex.glsl"));
  shader_builder.set_shader(GL_FRAGMENT_SHADER,
                            std::string("//shaders//fragment.glsl"));
  const GLuint shader = shader_builder.build();
  glUseProgram(shader);

  //------------------------------------------------------------ Main Loop --//

  // Clear the background at least once before rendering the GUI
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // TODO: create a close callback to handle cleaning up
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    gui::render(window);

    // Swap the front and back buffers
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // TODO: select load video from disk
  // TODO: detect(?) and set frame rate
  // TODO: play the original and stabilized video for side-by-side comparison
  // TODO: save video to disk

  // Happy path: clean up and exit
  app::shutdown(window);

  return EXIT_SUCCESS;
}
