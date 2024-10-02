#ifndef LOGGER_H
#define LOGGER_H

#include "imgui.h"
#include "utils.h"

#include <mutex>

class logger {
  std::mutex mutex_;

protected:
  explicit logger();
  ~logger();

  bool auto_scroll_ = true;
  ImGuiTextBuffer buf_;
  // Index to lines offset. We maintain this with add_log() calls.
  ImVector<int> line_offsets_;
  static constexpr float footer_buffer = 38.0f;

public:
  // Delete unused constructors and assignment operators
  logger(logger const& other) = delete;              // Copy Constructor
  logger(logger const&& other) = delete;             // Move Constructor
  logger& operator=(const logger& other) = delete;   // Copy-Assignment Operator
  logger& operator=(const logger&& other) = delete;  // Move-Assignment Operator

  static auto instance() -> logger*; 

  auto clear_log() -> void;

  auto add_log(const char* fmt, ...) -> void IM_FMTARGS(2);

  auto draw() -> void;
};

#endif  // LOGGER_H
