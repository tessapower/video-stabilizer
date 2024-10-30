#ifndef VIDEO_H
#define VIDEO_H

#include <filesystem>
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>

namespace vid {
class video {
 public:
  video();
  explicit video(std::filesystem::path const& video_file_path);
  video(video const& other);      // Copy Constructor
  video(video&& other) noexcept;  // Move Constructor
  ~video() = default;

  video& operator=(video const& other) = default;  // Copy-Assignment Operator

  auto load_video_from_file(
      std::filesystem::path const& video_file_path) noexcept -> void;

  /**
   * @brief Exports the stabilized video to the given file path.
   */
  [[nodiscard]] auto export_to_file(std::string const& save_dir) const noexcept
      -> bool;

  [[nodiscard]] auto empty() const noexcept -> bool {
    return frame_count_ == 0;
  }

  [[nodiscard]] auto fps() const noexcept -> int { return fps_; }

  [[nodiscard]] auto fourcc() const noexcept -> int { return fourcc_; }

  [[nodiscard]] auto bitrate() const noexcept -> double { return bitrate_; }

  [[nodiscard]] auto frame_count() const noexcept -> int {
    return frame_count_;
  }

  [[nodiscard]] auto frames() const noexcept -> std::vector<cv::Mat> {
    return frames_;
  }

  auto frames(std::vector<cv::Mat> const& new_frames) noexcept -> void {
    frames_ = new_frames;
    frame_count_ = static_cast<int>(frames_.size());
    size_ = frames_[0].size();
  }

  [[nodiscard]] auto clone() const noexcept -> video;

 private:
  std::string file_name_;
  std::vector<cv::Mat> frames_{};
  double bitrate_ = 0;
  int fourcc_ = 0;
  int fps_ = 0;
  int frame_count_ = 0;
  cv::Size size_;

  auto process_video(std::filesystem::path const& video_file_path) noexcept -> void;

  auto load_frames(std::vector<std::string> const& frames_file_paths) noexcept -> void;

  static auto padded_string(int n, int frame_count) noexcept -> std::string;
};
}  // namespace vid

#endif  // VIDEO_H
