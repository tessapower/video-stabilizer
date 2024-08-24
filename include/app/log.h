#ifndef LOG_H
#define LOG_H

#include <iostream>

#include "imgui.h"
#include "utils.h"

namespace gui {
struct log {
  ImGuiTextBuffer buf;
  // Index to lines offset. We maintain this with add_log() calls.
  ImVector<int> line_offsets;
  bool auto_scroll = true;
  static constexpr float footer_buffer = 38.0f;

  log() { clear_log(); }

  ~log() { clear_log(); }

  auto clear_log() -> void { buf.clear(); }

  auto add_log(const char* fmt, ...) -> void IM_FMTARGS(2) {
    int old_size = buf.size();
    va_list args;
    va_start(args, fmt);
    buf.appendfv(fmt, args);
    va_end(args);
    for (const int new_size = buf.size(); old_size < new_size; old_size++)
      if (buf[old_size] == '\n') line_offsets.push_back(old_size + 1);
  }

  auto draw() -> void {
    // Options menu
    if (ImGui::BeginPopup("Options")) {
      ImGui::Checkbox("Auto-scroll", &auto_scroll);
      ImGui::EndPopup();
    }

    // Buttons
    if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
    ImGui::SameLine();

    // Create a disabled button if the buffer is empty
    ImGui::BeginDisabled(buf.empty());
    const bool clear = ImGui::Button("Clear");
    ImGui::EndDisabled();

    ImGui::Separator();

    // Text area
    const float footer_height = ImGui::GetStyle().ItemSpacing.y +
                                ImGui::GetFrameHeightWithSpacing() +
                                footer_buffer;
    if (ImGui::BeginChild("scrolling", ImVec2(0, -footer_height),
                          ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar)) {
      if (clear) clear_log();

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

      ImGui::TextUnformatted(buf.begin(), buf.end());
    }
    ImGui::PopStyleVar();

    // Keep up at the bottom of the scroll region if we were already at the
    // bottom at the beginning of the frame. Using a scrollbar or mouse-wheel
    // will take away from the bottom edge.
    if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
      ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::Separator();
  }
};
}  // namespace gui

#endif  // LOG_H
