#ifndef APP_H
#define APP_H

// Always ensure GLAD is included before GLFW or face compiler errors!
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.h>

#include <iostream>

#include "logger/logger.h"
#include "video/stabilizer.h"
#include "model.h"

namespace app {
static constexpr int window_width = 500;
static constexpr int window_height = 600;

static GLFWwindow *window;

static model mod;

static std::thread worker;

static vid::stabilizer stabilizer;

inline auto loading_char() -> std::string {
  return std::string{"|/-\\"[static_cast<int>(ImGui::GetTime() / 0.05f) & 3]};
}

inline auto state_changed(const state old_state, const state new_state)
    -> void {
  switch (old_state) {
    case state::waiting: {
      switch (new_state) {
        case state::loading: {
          logger::instance()->add_dynamic_log("Loading", []() -> std::string {
            return "Loading " + loading_char() + "\n";
          });
          break;
        }
        case state::stabilizing: {
          logger::instance()->add_dynamic_log(
              "Stabilizing", []() -> std::string {
                return "Stabilizing " + loading_char() + "\n";
              });
          break;
        }
        case state::saving: {
          logger::instance()->add_log("Saving video...\n");
          break;
        }
        case state::waiting:
          break;
      }

      break;
    }
    case state::loading: {
      if (new_state != state::waiting) {
        // TODO: introduce custom error
        // Some kind of error!
      }
      logger::instance()->remove_dynamic_log("Loading");
      if (mod.did_load()) {
        logger::instance()->add_log("Video loaded!\n");
        logger::instance()->add_log("File path: \"%s\"\n",
                                    mod.video_path.c_str());

        logger::instance()->add_log("  - FPS: %i\n", mod.video->fps());

        const auto fourcc = mod.video->fourcc();
        // Transform from int to char via Bitwise operators
        char encoding[] = {static_cast<char>(fourcc & 0XFF),
                           static_cast<char>((fourcc & 0XFF00) >> 8),
                           static_cast<char>((fourcc & 0XFF0000) >> 16),
                           static_cast<char>((fourcc & 0XFF000000) >> 24), 0};
        logger::instance()->add_log("  - CODEC: %s\n", encoding);
        logger::instance()->add_log("  - Bitrate: %f kbits/sec\n",
                                    mod.video->bitrate());
      } else {
        logger::instance()->add_log("Error: video could not be loaded :(");
      }

      break;
    }
    case state::stabilizing: {
      if (new_state != state::waiting) {
        // Some kind of error!
      }
      logger::instance()->remove_dynamic_log("Stabilizing");
      logger::instance()->add_log(
          "%s\n",
          (mod.is_stabilized() ? "Video stabilized!"
                               : "Error: video could not be stabilized :("));
      break;
    }
    case state::saving: {
      if (new_state != state::waiting) {
        // Some kind of error!
      }

      logger::instance()->add_log(
          "%s\n", (mod.did_save() ? "Video saved!"
                                  : "Error: video could not be saved :("));
      break;
    }
  }
}

inline auto on_load_clicked() -> void {
  // Ensure the previous thread has finished before starting a
  // new one
  if (worker.joinable()) worker.join();
  if (utils::get_video_path(window, mod.video_path)) {
    mod.transition_to_state(state::loading);
    worker = std::thread(
        [](model &m) {
          // Create a new video object if it doesn't exist
          if (!m.video) {
            m.video = new vid::video(m.video_path);
          } else {
            m.video->load_video_from_file(m.video_path);
          }

          // If we failed to load a video, reset the pointer
          if (m.video && m.video->empty()) {
            m.video = nullptr;
            m.video_path = "";
          } else {
            m.last_save_successful = false;
            m.video_stabilized = false;
            m.save_dir = "";
          }

          m.transition_to_state(state::waiting);
        },
        std::ref(mod));
  }
}

inline auto on_stabilize_clicked() -> void {
  if (worker.joinable()) worker.join();

  mod.transition_to_state(state::stabilizing);

  worker = std::thread(
      [](model &m) {
        m.video_stabilized = m.video->stabilize();

        m.transition_to_state(state::waiting);
      },
      std::ref(mod));
}

inline auto on_save_clicked() -> void {
  if (worker.joinable()) worker.join();

  mod.transition_to_state(state::saving);
  if (utils::get_save_directory(mod.save_dir)) {
    worker = std::thread(
        [&](model &m) {
          m.last_save_successful = m.video->export_to_file(m.save_dir);

          m.transition_to_state(state::waiting);
        },
        std::ref(mod));
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
inline auto init_imgui() -> bool {
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
inline auto shutdown() -> void {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  NFD_Quit();

  glfwDestroyWindow(window);
  glfwTerminate();

  if (worker.joinable()) worker.join();
}
}  // namespace app

#endif  // APP_H
