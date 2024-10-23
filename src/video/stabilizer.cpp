#include "video/stabilizer.h"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include "logger/logger.h"

namespace vid {
//----------------------------------------------------------------- Public --//
auto stabilizer::stabilize() noexcept -> void {
  // Generate the H matrices for all frame pairs
  generate_h_mats();

  // Calculate the cumulative transformation matrices
  compute_h_tilde();

  // Smooth out the cumulative transformation matrices
  compute_h_tilde_prime();

  // Calculate the update transformation matrices
  compute_update_transforms();

  // Apply the corresponding update transformation matrices to each frame
  stabilize_frames();
}

auto stabilizer::crop_frames() noexcept -> void {
  // If there are no stabilized frames, don't do anything.
  if (stabilized_frames_.empty()) return;

  // Create a white mask
  cv::Mat white_mask(stabilized_frames_[0].size(), CV_8UC1, cv::Scalar(1.0));
  cv::Mat mask = white_mask.clone();
  const auto size = static_cast<int>(stabilized_frames_.size());
  for (auto i = 0; i < size; ++i) {
    cv::Mat transformed;
    cv::warpPerspective(white_mask, transformed, update_transforms_[i],
                        white_mask.size(), 1, cv::BORDER_CONSTANT,
                        cv::Scalar(0.0));

    mask = mask.mul(transformed);
  }

  // Convert mask to square shape by using the smallest of the dimensions
  const auto min_dim = min(mask.rows, mask.cols);
  mask = mask(cv::Rect(0, 0, min_dim, min_dim));

  // Find the largest inscribed square of all the stabilized frames, starting
  // from the bottom-right corner
  // Initialize a matrix with the same size as the mask and initial values of
  // 0, i.e. black
  cv::Mat s(mask.size(), CV_32SC1, cv::Scalar(0.0));
  for (auto row = mask.rows - 1; row > 0; --row) {
    for (auto col = mask.cols - 1; col > 0; --col) {
      if (mask.at<uchar>(row, col) == 0) continue;

      // If we're dealing with the bottom-right corner, we can't use the
      // bottom, right, or bottom-right cells, so we just set the value to 1
      if (row == mask.rows - 1 || col == mask.cols - 1) {
        s.at<int>(row, col) = 1;
        continue;
      }

      // Otherwise, calculate the value of this cell by following the formula:
      // S[x, y] = min(S[x + 1, y], S[x, y + 1], S[x + 1, y + 1]) + 1
      s.at<int>(row, col) =
          min(s.at<int>(row + 1, col),
              min(s.at<int>(row, col + 1), s.at<int>(row + 1, col + 1))) +
          1;
    }
  }

  // Create a region that represents the largest inscribed square
  double square_min, square_max;
  cv::Point square_min_idx, square_max_idx;
  // Find the global min and max in the count matrix
  cv::minMaxLoc(s, &square_min, &square_max, &square_min_idx, &square_max_idx);
  cv::Rect square(square_max_idx, cv::Size(static_cast<int>(square_max),
                                           static_cast<int>(square_max)));

  // Scale the square region
  const cv::Point2f scale(
      static_cast<float>(white_mask.cols) / static_cast<float>(mask.cols),
      static_cast<float>(white_mask.rows) / static_cast<float>(mask.rows));

  cv::Rect scaled_square(
      cv::Point2i(static_cast<int>(scale.x * square_max_idx.x),
                  static_cast<int>(scale.y * square_max_idx.y)),
      cv::Size2i(static_cast<int>(scale.x * square.width),
                 static_cast<int>(scale.y * square.height)));

  // Crop the stabilized frames to the largest inscribed square
  for (auto& frame : stabilized_frames_) frame = frame(scaled_square);
}

auto stabilizer::stabilized_frames() const noexcept -> std::vector<cv::Mat> {
  return stabilized_frames_;
}

auto stabilizer::generate_h_mats() noexcept -> void {
  // Clear any existing homography matrices
  h_mats_.clear();
  // Ensure the vector has enough space for (frames + 1) matrices
  const auto size = static_cast<int>(frames_.size());
  h_mats_.reserve(size);

  // Add the identity matrix first
  h_mats_.push_back(cv::Mat::eye(3, 3, CV_64FC1));

  logger::instance()->add_dynamic_log("h-mats", []() -> std::string {
    return std::string("Generating homography matrices") +
           utils::loading_dots() + "\n";
  });

  // Calculate the homography matrices for all frame pairs
  for (auto i = 1; i < size; ++i) {
    // Get the current and previous frames
    const auto img_1 = frames_[i];
    const auto img_2 = frames_[i - 1];

    ft_.set_images(img_1, img_2);
    ft_.track();
    h_mats_.push_back(ft_.h_mat());
  }

  logger::instance()->remove_dynamic_log("h-mats");
}

//---------------------------------------------------------------- Private --//

auto stabilizer::compute_h_tilde() noexcept -> void {
  h_tilde_.clear();

  // The first transformation matrix is always the identity matrix, which is
  // the first entry in the h_mats_ vector.
  h_tilde_.push_back(h_mats_.front());

  logger::instance()->add_dynamic_log("h-tilde", []() -> std::string {
    return std::string("Calculating cumulative transformation matrices") +
           utils::loading_dots() + "\n";
  });

  // Calculate the cumulative transformation matrices
  const auto size = static_cast<int>(h_mats_.size());
  for (auto i = 1; i < size; ++i) {
    h_tilde_.push_back(h_tilde_[i - 1] * h_mats_[i]);
  }

  logger::instance()->remove_dynamic_log("h-tilde");
}

auto stabilizer::compute_h_tilde_prime() noexcept -> void {
  h_tilde_prime_.clear();

  logger::instance()->add_dynamic_log("h-tilde-prime", []() -> std::string {
    return std::string("Applying filter to cumulative matrices") +
           utils::loading_dots() + "\n";
  });

  const auto size = static_cast<int>(h_tilde_.size());
  for (auto i = 0; i < size; ++i) {
    double sum = 0.0;
    cv::Mat h(3, 3, CV_64FC1, cv::Scalar(0.0));

    // Apply the filter to each cumulative transformation matrix
    const auto filter_size = static_cast<int>(weight_list_.size());
    for (auto j = 0; j < filter_size; ++j) {
      const auto idx = i + j - 2;
      // If we're too close to the first or last frame, we can't use the
      // filter
      if (idx < 0 || idx >= size) continue;
      h += h_tilde_[idx].mul(weight_list_[j]);
      sum += weight_list_[j];
    }

    h_tilde_prime_.push_back(h.mul(1.0 / sum));
  }

  logger::instance()->remove_dynamic_log("h-tilde-prime");
}

auto stabilizer::compute_update_transforms() noexcept -> void {
  update_transforms_.clear();

  logger::instance()->add_dynamic_log("update-transforms", []() -> std::string {
    return std::string("Computing update transforms") + utils::loading_dots() +
           "\n";
  });

  // Ensure the update_transforms vector has enough
  // space for the number of frames
  const auto size = static_cast<int>(frames_.size());

  for (auto i = 0; i < size; ++i) {
    // U_i = H~'_i^-1 * H~_i
    update_transforms_.push_back(h_tilde_prime_[i].inv() * h_tilde_[i]);
  }

  logger::instance()->remove_dynamic_log("update-transforms");
}

auto stabilizer::stabilize_frames() noexcept -> void {
  stabilized_frames_.clear();

  logger::instance()->add_dynamic_log("stabilize-frames", []() -> std::string {
    return std::string("Stabilizing frames") + utils::loading_dots() + "\n";
  });

  const auto size = static_cast<int>(frames_.size());
  stabilized_frames_.reserve(size);

  for (auto i = 0; i < size; ++i) {
    cv::Mat stabilized_frame;

    cv::warpPerspective(frames_[i], stabilized_frame, update_transforms_[i],
                        frames_[i].size(), 1, cv::BORDER_CONSTANT);
    stabilized_frames_.push_back(stabilized_frame);
  }

  logger::instance()->remove_dynamic_log("stabilize-frames");
}
}  // namespace vid
