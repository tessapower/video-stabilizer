#include "video/vid.h"

#include <opencv2/core/core_c.h>

#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include "video/stabilizer.h"

namespace vid {
video::video(std::string const& video_file_path) {
  load_video_from_file(video_file_path);
}

auto video::load_frames(
    std::vector<std::string> const& frames_file_paths) noexcept -> void {
  // Clear out old data
  original_frames_.clear();

  for (const auto& path : frames_file_paths) {
    cv::Mat frame = cv::imread(path);
    if (frame.empty()) {
      std::cerr << "Error: Could not read image at \"" << path << "\"\n";
      continue;
    }

    original_frames_.push_back(frame);
  }
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
  original_frames_.clear();

  // Create a VideoCapture Object
  auto video = cv::VideoCapture(video_file_path);

  if (!video.isOpened()) {
    std::cerr << "Error: Could not open video file\n";

    return;
  }

  fps_ = static_cast<int>(video.get(cv::CAP_PROP_FPS));
  fourcc_ = static_cast<int>(video.get(cv::CAP_PROP_FOURCC));

  std::cout << "Opened video file: " << video_file_path << "\n";
  std::cout << "FPS: " << fps_ << "\n";
  const auto frame_count =
      static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
  std::cout << "Frame Count: " << frame_count << "\n";

  // Create a tmp directory to store the frames
  if (!std::filesystem::exists(".//tmp")) {
    std::filesystem::create_directory(".//tmp");
  }

  // Extract the frames from the video into the tmp directory
  cv::Mat frame;
  auto i = 0;
  while (video.read(frame)) {
    const auto file_name = std::string{
        ".//tmp//frame_" + padded_string(i++, frame_count) + ".png"};
    cv::imwrite(file_name, frame);
  }

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
  stabilizer_.stabilize();
  stabilizer_.crop_frames();

  if (stabilizer_.stabilized_frames().empty()) {
    std::cerr << "Could not stabilize video\n";

    return false;
  }

  return true;
}

auto video::export_to_file(std::string const& save_dir) const noexcept -> bool {
  const auto stabilized_frames = stabilizer_.stabilized_frames();
  if (stabilized_frames.empty()) {
    std::cerr << "Error: No stabilized frames to export\n";
    return false;
  }

  const auto save_location = std::string{save_dir + "/stabilized_video_0.avi"};
  const auto dimensions = stabilized_frames[0].size();

  // TODO: convert to logs
  std::cout << "FPS: " << fps_ << "\n";
  std::cout << "FOURCC Codec : " << fourcc_ << "\n";
  std::cout << "Dimensions: " << dimensions << "\n";

  // Create a VideoWriter object
  cv::VideoWriter writer;
  // TODO: Use codec based on platform, currently using "DIVX" for Windows.
  const auto fourcc = cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
  writer.open(save_location, fourcc, fps_, dimensions, true);

  if (!writer.isOpened()) {
    // TODO: convert to logs
    std::cerr << "Error: Could not open the output video file to write.\n";

    return false;
  }

  // TODO: convert to log statements
  std::cout << "Using " << writer.getBackendName() << " to write new file.\n";
  std::cout << stabilized_frames.size() << " frames to write\n";

  for (auto const& frame : stabilized_frames) {
    // Encode the frame into the video file stream
    writer.write(frame);
  }

  return true;
}

auto video::n_frames() const noexcept -> size_t {
  return original_frames_.size();
}
}  // namespace vid
