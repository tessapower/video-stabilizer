#ifndef VIDEO_STABILIZER_H
#define VIDEO_STABILIZER_H

#include <opencv2/core/mat.hpp>

#include "vid.h"
#include "image/feature_tracker.h"

namespace vid {
class stabilizer {
 public:
  explicit stabilizer() = default;

  /**
   * @brief Stabilizes the video frames.
   */
  auto stabilize(video const* in, video* out) noexcept -> bool;

 private:
  // Original and stabilized frames
  std::vector<cv::Mat> frames_;
  std::vector<cv::Mat> stabilized_frames_;

  // Feature Tracker and H Transforms
  img::feature_tracker ft_;

  std::vector<cv::Mat> h_mats_;
  std::vector<cv::Mat> h_tilde_;

  // Local filter window
  std::vector<double> weight_list_{0.1, 0.3, 0.5, 0.3, 0.1};
  std::vector<cv::Mat> h_tilde_prime_;

  std::vector<cv::Mat> update_transforms_;

  /**
   * @brief Generates the homography matrices for all frame pairs.
   */
  auto generate_h_mats() noexcept -> void;

  /**
   * @brief Computes the cumulative transformation matrices.
   */
  auto compute_h_tilde() noexcept -> void;

  /**
   * @brief Computes the smoothed out the cumulative transformation matrices.
   */
  auto compute_h_tilde_prime() noexcept -> void;

  /**
   * @brief Computes the update transformation matrices.
   */
  auto compute_update_transforms() noexcept -> void;

  /**
   * @brief Stabilizes the frames using the update transformation matrices.
   */
  auto stabilize_frames() noexcept -> void;

  /**
   * @brief Crops the stabilized frames to remove borders. Assumes that
   * <code>stabilize()</code> has been called.
   */
  auto crop_frames() noexcept -> void;
};
}  // namespace vid

#endif  // VIDEO_STABILIZER_H
