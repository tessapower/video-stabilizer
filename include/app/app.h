#ifndef APP_H
#define APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <nfd.h>

#include <iostream>

#include "model.h"

namespace app {
static constexpr int window_width = 500;
static constexpr int window_height = 600;

static model mod;

inline auto state_changed(const state old_state, const state new_state)
    -> void {
  switch (old_state) {
    case state::waiting: {
      switch (new_state) {
        case state::loading: {
          log::instance()->add_log("Loading video...\n");
          break;
        }
        case state::stabilizing: {
          log::instance()->add_log("Stabilizing video...\n");
          break;
        }
        case state::saving: {
          log::instance()->add_log("Saving video...\n");
          break;
        }
        case state::waiting:
          break;
      }

      break;
    }
    case state::loading: {
      if (new_state != state::waiting) {
        // Some kind of error!
      }

      log::instance()->add_log(
          "%s\n", (mod.did_load() ? "Video loaded!\n"
                                  : "Error: video could not be loaded :(\n"));
      break;
    }
    case state::stabilizing: {
      if (new_state != state::waiting) {
        // Some kind of error!
      }

      log::instance()->add_log(
          "%s\n",
          (mod.is_stabilized() ? "Video stabilized!\n"
                               : "Error: video could not be stabilized :(\n"));
      break;
    }
    case state::saving: {
      if (new_state != state::waiting) {
        // Some kind of error!
      }

      log::instance()->add_log(
          "%s\n", (mod.did_save() ? "Video saved!\n"
                                  : "Error: video could not be saved :(\n"));
      break;
    }
  }
}


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

  // TODO: convert to debug logs
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

  // Set flags and styles
  ImGui::StyleColorsDark();

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
inline auto shutdown(GLFWwindow *window) -> void {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  NFD_Quit();

  glfwDestroyWindow(window);
  glfwTerminate();
}
}  // namespace app

#endif  // APP_H
