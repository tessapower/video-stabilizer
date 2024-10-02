#ifndef MODEL_H
#define MODEL_H

#include <atomic>
#include <functional>
#include <future>

#include "logger/logger.h"
#include "video/vid.h"

namespace app {
enum class state {
  waiting,     // Default state
  loading,     // Loading state
  saving,      // Saving state
  stabilizing  // Stabilizing state
};

class model {
public:
  std::atomic<state> current_state;
  std::atomic<bool> video_stabilized = false;
  std::atomic<bool> last_save_successful = false;
  std::function<void(state, state)> state_change_cb;
  vid::video* video;
  std::string video_path;
  std::string save_dir;

  model() : current_state(state::waiting), video(nullptr) {}

  ~model() { delete video; }

  auto state() const noexcept -> state { return current_state; }

  auto did_load() const noexcept -> bool { return video != nullptr; }

  auto did_save() const noexcept -> bool { return last_save_successful; }

  auto is_stabilized() const noexcept -> bool { return video_stabilized; }

  auto set_state_change_cb(
      const std::function<void(app::state, app::state)>& callback) {
    state_change_cb = callback;
  }

  auto transition_to_state(const app::state new_state) -> void {
    const auto old_state = current_state.load();
    current_state.store(new_state);

    if (state_change_cb) state_change_cb(old_state, new_state);
  }
};
}  // namespace app

#endif  // MODEL_H
