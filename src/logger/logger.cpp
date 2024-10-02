#include <logger/logger.h>

logger::logger() { clear_log(); }

logger::~logger() { clear_log(); }

auto logger::instance() -> logger* {
  // Static local variable initialization is thread-safe
  // and happens only once.
  static logger instance{};

  return &instance;
}

auto logger::clear_log() -> void { buf_.clear(); }

auto logger::add_log(const char* fmt, ...) -> void IM_FMTARGS(2) {
  std::lock_guard lock(mutex_);

  int old_size = buf_.size();
  va_list args;
  va_start(args, fmt);
  buf_.appendfv(fmt, args);
  va_end(args);
  for (const int new_size = buf_.size(); old_size < new_size; old_size++)
    if (buf_[old_size] == '\n') line_offsets_.push_back(old_size + 1);
}

auto logger::draw() -> void IM_FMTARGS(2) {
  std::lock_guard lock(mutex_);

  // Options menu
  if (ImGui::BeginPopup("Options")) {
    ImGui::Checkbox("Auto-scroll", &auto_scroll_);
    ImGui::EndPopup();
  }

  // Buttons
  if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
  ImGui::SameLine();

  // Create a disabled button if the buffer is empty
  ImGui::BeginDisabled(buf_.empty());
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

    ImGui::TextUnformatted(buf_.begin(), buf_.end());
  }
  ImGui::PopStyleVar();

  // Keep up at the bottom of the scroll region if we were already at the
  // bottom at the beginning of the frame. Using a scrollbar or mouse-wheel
  // will take away from the bottom edge.
  if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();

  ImGui::Separator();
}
