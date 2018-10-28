#include "opencv2/imgproc/imgproc.hpp"

void getSift(const cv::Ptr<cv::Feature2D>& f2d, const cv::Mat& image, vector<cv::Mat>& features) {
  // 1600 2276
  cv::Mat dst;
  cv::resize(image, dst, cv::Size(560, 800), 0, 0);
  std::vector<cv::KeyPoint> keypoints;
  f2d->detect( dst, keypoints );

  cv::Mat descriptors;
  f2d->compute( dst, keypoints, descriptors );
  features.resize(descriptors.rows);
  for(int i = 0; i < descriptors.rows; ++i) {
    features[i] = descriptors.row(i);
  }
}

void getOrb(const cv::Ptr<cv::ORB>& orb, const cv::Mat& image, vector<cv::Mat>& features) {
    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    orb->detectAndCompute(image, mask, keypoints, descriptors);

    features.resize(descriptors.rows);
    for(int i = 0; i < descriptors.rows; ++i)
    {
      features[i] = descriptors.row(i);
    }
}

void getSurf(const cv::Ptr<cv::xfeatures2d::SURF>& surf, const cv::Mat& image, vector<cv::Mat>& features) {
  cv::Mat dst;
  cv::resize(image, dst, cv::Size(1024, 1600), 0, 0);
  std::vector<cv::KeyPoint> keypoints;
  surf->detect( dst, keypoints );

  cv::Mat descriptors;
  surf->compute( dst, keypoints, descriptors );
  int rows = descriptors.rows;
  int cols = descriptors.cols;
  cout << "descriptor size " << rows << " " << cols << endl;
  features.resize(descriptors.rows);
  for(int i = 0; i < descriptors.rows; ++i) {
    features[i] = descriptors.row(i);
  }
}

void mat2vector(const cv::Mat& vec, vector<float>& result) {
  assert(vec.rows == 1);
  result.resize(vec.cols);
  for (int k = 0; k < vec.cols; k++) {
    result[k] = vec.at<float>(0, k);
  }
}


// ----------------------------------------------------------------------------
