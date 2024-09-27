#ifndef VIDEO_H
#define VIDEO_H

#include <opencv2/core/mat.hpp>

#include "stabilizer.h"

namespace vid {
class video {
 public:
  video();
  explicit video(std::string const& video_file_path);
  video& operator=(const video& other) noexcept;  // Copy-Assignment Operator
  ~video() = default;

  // Delete unused constructors and assignment operators
  video(video const& other) = delete; // Copy Constructor
  video(video const&& other) = delete; // Move Constructor
  video& operator=(const video&& other) = delete; // Move-Assignment Operator

  auto load_video_from_file(std::string const& video_file_path) noexcept
      -> void;

  /**
   * @brief Stabilizes the video.
   */
  auto stabilize() noexcept -> bool;

  /**
   * @brief Exports the stabilized video to the given file path.
   */
  [[nodiscard]] auto export_to_file(std::string const& save_dir) const noexcept
      -> bool;

  [[nodiscard]] auto empty() const noexcept -> bool {
    return original_frames_.empty();
  }

 private:
  std::vector<cv::Mat> original_frames_{};
  vid::stabilizer stabilizer_{};
  int fps_ = 0;
  int fourcc_ = 0;

  auto load_frames(std::vector<std::string> const& frames_file_paths) noexcept
      -> void;

  static auto padded_string(const int n, const int frame_count) noexcept
      -> std::string;
};
}  // namespace vid

#endif  // VIDEO_H
