#ifndef LOGGER_H
#define LOGGER_H

#include "imgui.h"
#include "utils.h"

#include <mutex>
#include <unordered_map>
#include <string>
#include <functional>

class logger {
  std::mutex mutex_;

  // Dynamic logs
  std::unordered_map<std::string, std::function<std::string()>> dynamic_logs_;

protected:
  explicit logger();
  ~logger();

  bool auto_scroll_ = true;
  bool clear_ = false;
  ImGuiTextBuffer buf_;
  // Index to lines offset. We maintain this with add_log() calls.
  ImVector<int> line_offsets_;
  static constexpr float footer_buffer = 38.0f;

  auto clear_log() -> void;

public:
  // Delete unused constructors and assignment operators
  logger(logger const& other) = delete;              // Copy Constructor
  logger(logger const&& other) = delete;             // Move Constructor
  logger& operator=(const logger& other) = delete;   // Copy-Assignment Operator
  logger& operator=(const logger&& other) = delete;  // Move-Assignment Operator

  static auto instance() -> logger*;

  auto add_log(const char* fmt, ...) -> void IM_FMTARGS(2);

  auto draw() -> void;
  
  auto add_dynamic_log(const std::string& id, std::function<std::string()> getter) -> void;

  auto remove_dynamic_log(const std::string& id) -> void;

  [[nodiscard]] auto empty() const noexcept -> bool;

  auto clear() noexcept -> void;

  auto set_auto_scroll(bool b) noexcept -> void;
};

#endif  // LOGGER_H
