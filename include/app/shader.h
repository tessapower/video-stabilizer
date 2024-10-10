#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace app {
class shader_error : public std::runtime_error {
 public:
  explicit shader_error(const std::string &what = "Generic shader error.")
      : std::runtime_error(what) {}
};

class shader_type_error final : public shader_error {
 public:
  explicit shader_type_error(const std::string &what = "Bad shader type.")
      : shader_error(what) {}
};

class shader_compile_error final : public shader_error {
 public:
  explicit shader_compile_error(
      const std::string &what = "Shader compilation failed.")
      : shader_error(what) {}
};

class shader_link_error final : public shader_error {
 public:
  explicit shader_link_error(
      const std::string &what = "Shader program linking failed.")
      : shader_error(what) {}
};

class shader_builder {
  std::map<GLenum, std::shared_ptr<GLuint>> shaders_;

 public:
  shader_builder() = default;

  auto set_shader(const GLenum type, const std::string &file_name) -> void {
    const std::ifstream file_stream(std::filesystem::current_path().string() + file_name);

    if (!file_stream) {
      // TODO: convert to debug log
      std::cerr << "Error: Could not locate and open file " << file_name
                << '\n';
      throw std::runtime_error("Error: Could not locate and open file " +
                               file_name);
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();

    try {
      set_shader_source(type, buffer.str());
    } catch (shader_compile_error &e) {
      // TODO: convert to debug log
      std::cerr << "Error: Could not compile " << file_name << '\n';
      std::cerr << "Shader Error: " << e.what() << "\n";
      throw;
    }
  }

  auto set_shader_source(const GLenum type, const std::string &shader_source)
      -> void {
    GLint shader = glCreateShader(type);

    const auto get_define = [](const GLenum s_type) {
      switch (s_type) {
        case GL_VERTEX_SHADER:
          return "_VERTEX_";
        case GL_GEOMETRY_SHADER:
          return "_GEOMETRY_";
        case GL_TESS_CONTROL_SHADER:
          return "_TESS_CONTROL_";
        case GL_TESS_EVALUATION_SHADER:
          return "_TESS_EVALUATION_";
        case GL_FRAGMENT_SHADER:
          return "_FRAGMENT_";
        default:
          return "_INVALID_SHADER_TYPE_";
      }
    };

    std::istringstream iss(shader_source);
    std::ostringstream oss;
    while (iss) {
      std::string line;
      std::getline(iss, line);
      oss << line << '\n';
      if (line.find("#version") < line.find("//")) break;
    }
    oss << "#define " << get_define(type) << '\n';
    oss << iss.rdbuf();
    const std::string final_source = oss.str();

    // Upload and compile shader
    const char *text_c = final_source.c_str();
    glShaderSource(shader, 1, &text_c, nullptr);
    glCompileShader(shader);

    // Check compilation status
    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    print_shader_info_log(shader);
    if (!compile_status) throw shader_compile_error();

    shaders_[type] = std::make_shared<GLuint>(shader);
  }

  [[nodiscard]] auto build(GLuint program = 0) const -> GLuint {
    // Detach existing shaders
    if (program) {
      int shader_count = 0;
      glGetProgramiv(program, GL_ATTACHED_SHADERS, &shader_count);

      if (shader_count > 0) {
        std::vector<GLuint> attached_shaders(shader_count);
        int actual_shader_count = 0;
        glGetAttachedShaders(program, shader_count, &actual_shader_count,
                             attached_shaders.data());
        for (int i = 0; i < actual_shader_count; i++) {
          glDetachShader(program, attached_shaders[i]);
        }
      }
    } else {
      program = glCreateProgram();
    }

    // Attach given shaders
    for (const auto &shader : shaders_ | std::views::values) {
      glAttachShader(program, *(shader));
    }
    glLinkProgram(program);

    // Check link status
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    print_program_info_log(program);
    if (!link_status) throw shader_link_error();

    return program;
  }

 private:
  static auto print_shader_info_log(const GLuint obj) -> void {
    int info_log_length = 0;
    int chars_written = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 1) {
      std::vector<char> info_log(info_log_length);
      glGetShaderInfoLog(obj, info_log_length, &chars_written, info_log.data());
      // TODO: convert to debug log
      std::cout << "SHADER:\n" << info_log.data() << '\n';
    }
  }

  static auto print_program_info_log(const GLuint obj) -> void {
    int info_log_length = 0;
    int chars_written = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 1) {
      std::vector<char> info_log(info_log_length);
      glGetProgramInfoLog(obj, info_log_length, &chars_written,
                          info_log.data());
      // TODO: convert to debug log
      std::cout << "PROGRAM:\n" << info_log.data() << '\n';
    }
  }
};
}  // namespace app

#endif  // SHADER_H
