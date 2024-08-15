#ifndef APP_H
#define APP_H

#include <glad/gl.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

namespace app {
/**
 * @brief Returns a string representing the error source.
 */
inline auto source_string(const GLenum source) -> const char * {
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "Application";
    case GL_DEBUG_SOURCE_OTHER:
      return "Other";
    default:
      return "Unknown";
  }
}

/**
 * @brief Returns a string representing the debug severity.
 */
inline auto severity_string(const GLenum severity) -> const char * {
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
      return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "None";
    default:
      return "Unknown";
  }
}

/**
 * @brief Returns a string representing the error type.
 */
inline auto type_string(const GLenum type) -> const char * {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "Deprecated Behaviour";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "Undefined Behaviour";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "Performance";
    case GL_DEBUG_TYPE_OTHER:
      return "Other";
    default:
      return "Unknown";
  }
}

/**
 * @brief A callback function for OpenGL debug messages.
 */
inline auto debug_cb(const GLenum source, const GLenum type, const GLuint id,
                     const GLenum severity, GLsizei, const GLchar *message,
                     const void *) -> void {
  // Don't report notification messages
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

  // nvidia: avoid debug spam about attribute offsets
  if (id == 131076) return;

  std::cerr << "GL [" << source_string(source) << "] " << type_string(type)
            << ' ' << id << " : ";
  std::cerr << message << " (Severity: " << severity_string(severity) << ")\n";
}

/**
 * Initializes ImGui system.
 * @return True if initialization was successful, otherwise false.
 */
inline auto init_imgui(GLFWwindow *window) -> bool {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460 core");
  ImGui::StyleColorsClassic();

  return true;
}

/**
 * Initializes GLFW window system and OpenGL context.
 * @return True if initialization was successful, otherwise false.
 */
inline auto init_glfw() -> bool {
  if (!glfwInit()) return false;

  // Create a 4.5 core context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Don't allow legacy functionality (OSX works better this way)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Request a debug context to get debug callbacks
  // Remove this for possible GL performance increases
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  return true;
}

/**
 * @brief Shuts down all appropriate systems.
 */
inline auto shutdown(GLFWwindow * window) -> void {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}
}  // namespace app

#endif  // APP_H
