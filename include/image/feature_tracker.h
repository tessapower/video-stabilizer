#ifndef FEATURE_TRACKER_H
#define FEATURE_TRACKER_H

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>

namespace img {
class feature_tracker {
 public:
  explicit feature_tracker() {
    sift_ = sift_->create();
    matcher_ = matcher_->create(cv::NORM_L2, true);
  }

  explicit feature_tracker(cv::Mat img_1, cv::Mat img_2)
      : img_1_{std::move(img_1)}, img_2_{std::move(img_2)} {
    sift_ = sift_->create();
    matcher_ = matcher_->create(cv::NORM_L2, true);
  }

  /**
   * \brief Sets the images.
   */
  auto set_images(cv::Mat img_1, cv::Mat img_2) noexcept -> void {
    img_1_ = std::move(img_1);
    img_2_ = std::move(img_2);
  }

  /**
   * \brief Detects and matches the features in the two images. This function
   * should be called <i>before</i> calling <code>visualize_matches()</code>,
   * or <code>visualize_match_quality()</code>. If images have recently been
   * set, this function should be called again.
   */
  auto track() noexcept -> void;

  /**
   * \brief Returns an image that is a combination of the two images based on
   * their matching feature points. Assumes that <code>track()</code> has
   * already been called.
   */
  auto warp_image() const noexcept -> cv::Mat;

  /**
   * \brief Returns the Hessian matrix that transforms the points in the
   * first image to the points in the second image. Assumes that
   * <code>track()</code> has already been called, otherwise returns an
   * empty matrix.
   */
  [[nodiscard]] auto h_mat() const noexcept -> cv::Mat { return h_mat_; }

 private:
  // The original images
  cv::Mat img_1_, img_2_;

  // Key points
  cv::Ptr<cv::SIFT> sift_{};
  std::vector<cv::KeyPoint> key_points_1_, key_points_2_;
  cv::Mat descriptors_1_, descriptors_2_;

  // Matches
  cv::Ptr<cv::BFMatcher> matcher_{};
  std::vector<cv::DMatch> matches_;

  // Colors
  static const cv::Scalar match_color;
  static const cv::Scalar inlier_color;
  static const cv::Scalar outlier_color;
  static const cv::Scalar border_color;

  // Hessian Matrix Values
  cv::Mat h_mat_;
  static constexpr float epsilon = 10.0f;

  // Warp Values
  static constexpr int border_size = 50;

  /**
   * \brief Detects the features in the two images.
   */
  auto detect_features() noexcept -> void;

  /**
   * \brief Matches the features in the two images.
   */
  auto match_features() noexcept -> void;

  /**
   * \brief Finds the best homography matrix that transforms the points from
   * the first image to the second image.
   */
  auto find_best_homography() noexcept -> void;

  /**
   * \brief Calculates the error between the match point and the transformed
   * point for the given match.
   */
  [[nodiscard]] auto calc_error(const cv::Mat& h_mat,
                                const cv::DMatch& match) const noexcept
      -> float;

  /**
   * \brief Transform a point by the given Hessian matrix. Returns the
   * transformed point.
   */
  [[nodiscard]] static auto h_transform(cv::Mat const& h,
                                        cv::Point2f const& point) noexcept
      -> cv::Point2f;

  /**
   * \brief Returns whether the given vector of <code>DMatch</code>es contains
   * the given <code>DMatch</code>.
   */
  static auto contains_match(std::vector<cv::DMatch> const& matches,
                             cv::DMatch const& match) -> bool;
};
}  // namespace img

#endif  // FEATURE_TRACKER_H
