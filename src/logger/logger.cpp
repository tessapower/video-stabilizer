#include <logger/logger.h>

logger::logger() { clear_log(); }

logger::~logger() { clear_log(); }

auto logger::instance() -> logger* {
  // Static local variable initialization is thread-safe
  // and happens only once.
  static logger instance{};

  return &instance;
}

auto logger::clear_log() -> void {
  buf_.clear();
  clear_ = false;
}

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

auto logger::add_dynamic_log(const std::string& id,
                          std::function<std::string()> getter) -> void {
  std::lock_guard lock(mutex_);
  dynamic_logs_[id] = std::move(getter);
}

auto logger::remove_dynamic_log(const std::string& id) -> void {
  std::lock_guard lock(mutex_);
  dynamic_logs_.erase(id);
}

auto logger::draw() -> void IM_FMTARGS(2) {
  std::lock_guard lock(mutex_);

  ImGui::Separator();

  // Text area
  const float footer_height = ImGui::GetStyle().ItemSpacing.y +
                              ImGui::GetFrameHeightWithSpacing() +
                              footer_buffer;

  if (ImGui::BeginChild("scrolling", ImVec2(0, -footer_height),
                        ImGuiChildFlags_None,
                        ImGuiWindowFlags_HorizontalScrollbar)) {
    if (clear_) clear_log();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

    ImGui::TextUnformatted(buf_.begin(), buf_.end());

    // Draw dynamic log entries
    for (const auto& [id, getter] : dynamic_logs_) {
      ImGui::TextUnformatted(getter().c_str());
    }
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

auto logger::empty() const noexcept -> bool { return buf_.empty(); }

auto logger::clear() noexcept -> void { clear_ = true; }

auto logger::set_auto_scroll(const bool b) noexcept -> void { auto_scroll_ = b; }
