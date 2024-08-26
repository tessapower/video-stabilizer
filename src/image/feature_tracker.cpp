#include "image/feature_tracker.h"

#include <opencv2/imgproc/imgproc_c.h>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace img {
const cv::Scalar feature_tracker::match_color{0.0, 255.0, 0.0};
const cv::Scalar feature_tracker::inlier_color{0.0, 255.0, 0.0};
const cv::Scalar feature_tracker::outlier_color{0.0, 0.0, 255.0};
const cv::Scalar feature_tracker::border_color{155.0, 155.0, 155.0};

auto feature_tracker::detect_features() noexcept -> void {
  // Clear the key points and descriptors
  key_points_1_.clear();
  key_points_2_.clear();
  descriptors_1_ = cv::Mat::zeros(0, 0, CV_32F);
  descriptors_2_ = cv::Mat::zeros(0, 0, CV_32F);

  // Detect the key points in the images
  sift_->detect(img_1_, key_points_1_);
  sift_->detect(img_2_, key_points_2_);

  // Compute the descriptors for the key points
  sift_->compute(img_1_, key_points_1_, descriptors_1_);
  sift_->compute(img_2_, key_points_2_, descriptors_2_);
}

auto feature_tracker::match_features() noexcept -> void {
  matcher_->match(descriptors_1_, descriptors_2_, matches_);
}

auto feature_tracker::track() noexcept -> void {
  // Don't do anything if the images are empty.
  if (img_1_.empty() || img_2_.empty()) return;

  // Detect the features in both images.
  detect_features();

  // Match the features to each other across the images
  match_features();

  // Compute the best homography matrix
  find_best_homography();
}

auto feature_tracker::contains_match(std::vector<cv::DMatch> const& matches,
                                     cv::DMatch const& match) -> bool {
  if (std::ranges::any_of(
          matches.cbegin(), matches.cend(), [&](cv::DMatch const& m) {
            return m.queryIdx == match.queryIdx &&
                   m.trainIdx == match.trainIdx && m.imgIdx == match.imgIdx;
          })) {
    return true;
  }

  return false;
}

auto feature_tracker::find_best_homography() noexcept -> void {
  // Estimate hessian matrix for random points
  std::vector<cv::DMatch> best_inliers;
  cv::RNG rng;
  for (auto i = 0; i < 1000; ++i) {
    // Select four random pairs of matches
    std::vector<cv::DMatch> random_matches;
    constexpr auto num_matches = 4;
    for (auto j = 0; j < num_matches; ++j) {
      const int random_idx = rng.uniform(0, static_cast<int>(matches_.size()));
      const auto& random_match = matches_[random_idx];

      if (contains_match(random_matches, random_match)) {
        --j;
      } else {
        random_matches.push_back(random_match);
      }
    }

    // Extract source and destination points from the matches
    std::vector<cv::Point2f> src_pts;
    std::vector<cv::Point2f> dst_pts;
    for (const auto& m : random_matches) {
      src_pts.push_back(key_points_1_[m.queryIdx].pt);
      dst_pts.push_back(key_points_2_[m.trainIdx].pt);
    }

    // Find the homography matrix for the pairs
    auto h_mat = cv::findHomography(src_pts, dst_pts);

    // Compute inlier pairs amongst all pairs, where the mapping error of the
    // transformed point q with the target position p is less than some epsilon
    // |p_i - H * q_i| < epsilon
    std::vector<cv::DMatch> inliers;
    for (const auto& m : matches_) {
      // If the error is less than epsilon, add the match to the inliers
      if (calc_error(h_mat, m) < epsilon) inliers.push_back(m);
    }

    // If the number of inlier pairs is greater than the previous iteration's,
    // save the homography transform and the list of best inliers
    if (inliers.size() > best_inliers.size()) {
      h_mat_ = h_mat;
      best_inliers = inliers;
    }
  }

  // Compute the final homography on the best matches
  std::vector<cv::Point2f> src_pts;
  std::vector<cv::Point2f> dst_pts;
  for (const auto& m : best_inliers) {
    src_pts.push_back(key_points_1_[m.queryIdx].pt);
    dst_pts.push_back(key_points_2_[m.trainIdx].pt);
  }

  // Set the homography matrix to the best homography matrix
  h_mat_ = cv::findHomography(src_pts, dst_pts);
}

auto feature_tracker::calc_error(const cv::Mat& h_mat,
                                 const cv::DMatch& match) const noexcept
    -> float {
  // Extract the points from the match
  const auto& p = key_points_1_[match.queryIdx].pt;
  const auto& q = key_points_2_[match.trainIdx].pt;

  // Compute the transformed point using the given Hessian matrix
  const auto q_prime = h_transform(h_mat, p);

  // Compute the error between the transformed point and the target point,
  // i.e. the distance between them
  return static_cast<float>(cv::norm(cv::Mat(q_prime), cv::Mat(q)));
}

auto feature_tracker::h_transform(cv::Mat const& h,
                                  cv::Point2f const& point) noexcept
    -> cv::Point2f {
  assert(h.type() == CV_64FC1);

  const auto p = cv::Vec3d(point.x, point.y, 1.0);
  const auto q = cv::Mat(h * cv::Mat(p));

  return {static_cast<float>(q.at<double>(0, 0)),
          static_cast<float>(q.at<double>(1, 0))};
}

auto feature_tracker::warp_image() const noexcept -> cv::Mat {
  cv::Mat img_1_border;
  cv::copyMakeBorder(img_1_, img_1_border, border_size, border_size,
                     border_size, border_size, cv::BORDER_CONSTANT,
                     border_color);

  cv::Mat img_2_border;
  cv::copyMakeBorder(img_2_, img_2_border, border_size, border_size,
                     border_size, border_size, cv::BORDER_CONSTANT,
                     border_color);

  cv::Mat warped_img = img_2_border.clone();

  cv::warpPerspective(img_1_border, warped_img, h_mat_, warped_img.size(), 1,
                      cv::BORDER_CONSTANT, border_color);

  const cv::Vec3b color{static_cast<uchar>(border_color[0]),
                        static_cast<uchar>(border_color[1]),
                        static_cast<uchar>(border_color[2])};

  for (auto row = 0; row < warped_img.rows; ++row) {
    for (auto col = 0; col < warped_img.cols; ++col) {
      if (img_2_border.at<cv::Vec3b>(row, col) == color)
        img_2_border.at<cv::Vec3b>(row, col) =
            warped_img.at<cv::Vec3b>(row, col);
    }
  }

  return img_2_border;
}
}  // namespace img
