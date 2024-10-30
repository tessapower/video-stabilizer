#include "video/vid.h"

#include <opencv2/core/core_c.h>

#include <ranges>
#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "logger/logger.h"

namespace vid {
video::video() {
  frames_ = std::vector<cv::Mat>{};

  bitrate_ = 0.0;
  frame_count_ = static_cast<int>(frames_.size());
  fourcc_ = 0;
  fps_ = 0;
  size_ = {0, 0};
}

video::video(video const& other) {
  if (this != &other) {
    std::ranges::copy(other.frames_,
                      std::back_inserter(frames_));
    file_name_ = other.file_name_;

    bitrate_ = other.bitrate_;
    fourcc_ = other.fourcc_;
    fps_ = other.fps_;
    frame_count_ = static_cast<int>(frames_.size());
    size_ = other.size_;
  }
}

video::video(video&& other) noexcept {
  if (this != &other) {
    std::ranges::move(other.frames_,
                      std::back_inserter(frames_));

    file_name_ = other.file_name_;
    other.file_name_ = "";
    bitrate_ = other.bitrate_;
    other.bitrate_ = 0;
    fourcc_ = other.fourcc_;
    other.fourcc_ = 0;
    fps_ = other.fps_;
    other.fps_ = 0;
    frame_count_ = static_cast<int>(frames_.size());
    other.frame_count_ = 0;
    size_ = other.size_;
    other.size_ = {0, 0};
  }
}

video::video(std::filesystem::path const& video_file_path) {
  file_name_ = video_file_path.filename().string();

  load_video_from_file(video_file_path.string());
}

auto video::load_frames(
    std::vector<std::string> const& frames_file_paths) noexcept -> void {
  // Clear out old data
  frames_.clear();

  logger::instance()->add_dynamic_log("load-frames", [&]() -> std::string {
    return std::string("Loading frames") + utils::loading_dots() + "\n";
  });

  for (const auto& path : frames_file_paths) {
    cv::Mat frame = cv::imread(path);
    if (frame.empty()) {
      // TODO: convert to debug log
      std::cerr << "Error: Could not read image at \"" << path << "\"\n";
      continue;
    }

    frames_.push_back(frame);
  }

  logger::instance()->remove_dynamic_log("load-frames");
}

auto video::padded_string(const int n, const int frame_count) noexcept
    -> std::string {
  std::string s = std::to_string(n);
  const auto fc_str = std::to_string(frame_count);

  while (s.length() < fc_str.length()) {
    s.insert(s.begin(), '0');
  }

  return s;
}

auto video::load_video_from_file(
    std::filesystem::path const& video_file_path) noexcept -> void {
  // Clear out old data
  if (!frames_.empty()) {
    frames_.clear();
    file_name_ = "";
    bitrate_ = 0.0;
    fourcc_ = 0;
    fps_ = 0;
    frame_count_ = 0;
    size_ = {0, 0};
  }

  process_video(video_file_path);

  // Read all the frames in the tmp folder into frames vector
  std::vector<cv::String> image_paths;
  cv::glob(".//tmp//", image_paths);

  load_frames(image_paths);

  // Delete all tmp files
  std::filesystem::remove_all(".//tmp//");
}

auto video::process_video(std::filesystem::path const& video_file_path) noexcept
    -> void {
  // Create a VideoCapture Object
  auto video_capture = cv::VideoCapture(video_file_path.string());

  if (!video_capture.isOpened()) {
    std::cerr << "Error: Could not open video file\n";

    return;
  }

  // Add dynamic log to track progress of video load
  logger::instance()->add_dynamic_log("progress", [&]() -> std::string {
    return std::string("Processing video") + utils::loading_dots() + "\n";
  });

  bitrate_ = video_capture.get(cv::CAP_PROP_BITRATE);
  fourcc_ = static_cast<int>(video_capture.get(cv::CAP_PROP_FOURCC));
  fps_ = static_cast<int>(video_capture.get(cv::CAP_PROP_FPS));
  frame_count_ = static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_COUNT));
  size_ =
      cv::Size(static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_WIDTH)),
               static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_HEIGHT)));

  // TODO: convert to debug log
  std::cout << "Opened video file: " << video_file_path.string() << "\n";
  std::cout << "FPS: " << fps_ << "\n";
  std::cout << "Frame Count: " << frame_count_ << "\n";

  // Create a tmp directory to store the frames
  if (!std::filesystem::exists(".//tmp")) {
    std::filesystem::create_directory(".//tmp");
  }

  // Extract the frames from the video into the tmp directory
  cv::Mat frame;

  auto i = 0;
  while (video_capture.read(frame)) {
    const auto tmp = std::string{".//tmp//frame_" +
                                 padded_string(i++, frame_count_) + ".png"};
    cv::imwrite(tmp, frame);
  }

  logger::instance()->remove_dynamic_log("progress");
}

auto video::export_to_file(std::string const& save_dir) const noexcept -> bool {
  if (frames_.empty()) {
    // TODO: convert to debug log
    std::cerr << "Error: No frames to export\n";

    return false;
  }

  // TODO: support user setting name of file
  const auto save_location = std::string{save_dir + "/video_0.avi"};
  const auto dimensions = frames_[0].size();

  // TODO: convert to debug log
  std::cout << "FPS: " << fps_ << "\n";
  std::cout << "FOURCC Codec : " << fourcc_ << "\n";
  std::cout << "Dimensions: " << dimensions << "\n";

  // Create a VideoWriter object
  cv::VideoWriter writer;
  // TODO: Use codec based on platform, currently using "DIVX" for Windows.
  const auto fourcc = cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
  writer.open(save_location, fourcc, fps_, dimensions, true);

  if (!writer.isOpened()) {
    // TODO: convert to debug log
    std::cerr << "Error: Could not open the output video file to write.\n";

    return false;
  }

  // TODO: convert to debug log
  std::cout << "Using " << writer.getBackendName() << " to write new file.\n";
  std::cout << frame_count_ << " frames to write\n";

  for (auto const& frame : frames_) {
    // Encode the frame into the video file stream
    writer.write(frame);
  }

  return true;
}

auto video::clone() const noexcept -> video {
  vid::video cloned{};

  std::ranges::copy(frames_,
                    std::back_inserter(cloned.frames_));

  cloned.file_name_ = file_name_;
  cloned.bitrate_ = bitrate();
  cloned.fourcc_ = fourcc();
  cloned.fps_ = fps();
  cloned.frame_count_ = frame_count();
  cloned.size_ = size_;

  return cloned;
}
}  // namespace vid
