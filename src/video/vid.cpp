#include "video/vid.h"

#include <opencv2/core/core_c.h>

#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "logger/logger.h"
#include "video/stabilizer.h"

namespace vid {
video::video() {
  original_frames_ = std::vector<cv::Mat>{};
  stabilizer_ = vid::stabilizer{};
  fps_ = 0;
  fourcc_ = 0;
}

video& video::operator=(video const& other) noexcept {
  if (this != &other) {
    original_frames_ = other.original_frames_;
    stabilizer_ = other.stabilizer_;
    fps_ = other.fps_;
    fourcc_ = other.fourcc_;
    size_ = other.size_;
  }

  return *this;
}

video::video(std::string const& video_file_path) {
  load_video_from_file(video_file_path);
}

auto video::load_frames(
    std::vector<std::string> const& frames_file_paths) noexcept -> void {
  // Clear out old data
  original_frames_.clear();

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

    original_frames_.push_back(frame);
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

auto video::load_video_from_file(std::string const& video_file_path) noexcept
    -> void {
  // Clear out old data
  if (!original_frames_.empty()) {
    original_frames_.clear();
    fps_ = 0;
    fourcc_ = 0;
  }

  // Create a VideoCapture Object
  auto video = cv::VideoCapture(video_file_path);

  if (!video.isOpened()) {
    std::cerr << "Error: Could not open video file\n";

    return;
  }

  bitrate_ = video.get(cv::CAP_PROP_BITRATE);
  fourcc_ = static_cast<int>(video.get(cv::CAP_PROP_FOURCC));
  fps_ = static_cast<int>(video.get(cv::CAP_PROP_FPS));
  frame_count_ = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
  size_ = cv::Size(static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH)),
                   static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT)));

  // TODO: convert to debug log
  std::cout << "Opened video file: " << video_file_path << "\n";
  std::cout << "FPS: " << fps_ << "\n";
  std::cout << "Frame Count: " << frame_count_ << "\n";

  // Create a tmp directory to store the frames
  if (!std::filesystem::exists(".//tmp")) {
    std::filesystem::create_directory(".//tmp");
  }

  // Extract the frames from the video into the tmp directory
  cv::Mat frame;

  // Add dynamic log to track progress of video load
  logger::instance()->add_dynamic_log("progress", [&]() -> std::string {
    return std::string("Processing video") + utils::loading_dots() + "\n";
  });

  auto i = 0;
  while (video.read(frame)) {
    const auto file_name = std::string{
        ".//tmp//frame_" + padded_string(i++, frame_count_) + ".png"};
    cv::imwrite(file_name, frame);
  }

  logger::instance()->remove_dynamic_log("progress");

  // Read all the frames in the tmp folder into frames vector
  std::vector<cv::String> image_paths;
  cv::glob(".//tmp//", image_paths);

  load_frames(image_paths);

  // Delete all tmp files
  std::filesystem::remove_all(".//tmp//");
}

auto video::stabilize() noexcept -> bool {
  // Pass frames to stabilizer and do the work
  stabilizer_.frames(original_frames_);
  // TODO: report progress to out var?
  stabilizer_.stabilize();
  stabilizer_.crop_frames();

  if (stabilizer_.stabilized_frames().empty()) {
    // TODO: convert to debug log
    std::cerr << "Error: could not stabilize video\n";

    return false;
  }

  return true;
}

auto video::export_to_file(std::string const& save_dir) const noexcept -> bool {
  const auto stabilized_frames = stabilizer_.stabilized_frames();
  if (stabilized_frames.empty()) {
    // TODO: convert to debug log
    std::cerr << "Error: No stabilized frames to export\n";
    return false;
  }

  // TODO: support user setting name of file
  const auto save_location = std::string{save_dir + "/stabilized_video_0.avi"};
  const auto dimensions = stabilized_frames[0].size();

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
  std::cout << stabilized_frames.size() << " frames to write\n";

  for (auto const& frame : stabilized_frames) {
    // Encode the frame into the video file stream
    writer.write(frame);
  }

  return true;
}
}  // namespace vid
