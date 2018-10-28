/**
 * File: Demo.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DBoW2
 * License: see the LICENSE.txt file
 */

#include <iostream>
#include <vector>

// DBoW2
#include "DBoW2.h" // defines OrbVocabulary and OrbDatabase

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "opencv2/xfeatures2d.hpp"


using namespace DBoW2;
using namespace std;

// ----------------------------------------------------------------------------
void testVocCreation(const vector<vector<cv::Mat > > &features, const string& saveto)
{
  // branching factor and depth levels
  const int k = 10;
  const int L = 6;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  //OrbVocabulary voc(k, L, weight, score);
  SurfVocabulary voc(k, L, weight, score);

  // conver to vector<vector<float>>
  vector<vector<vector<float>>> surf_features(features.size());
  for (int i = 0; i < features.size(); ++i) {
    surf_features[i].resize(features[i].size());
    cout << "featurep[" << i << "] has " << features[i].size() << endl;
    for (int j = 0; j < features[i].size(); ++j) {
      //cout << "copy descriptor size " << features[i][j].rows << " " << features[i][j].cols << endl;
      //assert(features[i][j].isContinuous);
      surf_features[i][j].resize(features[i][j].cols);
      for (int k = 0; k < features[i][j].cols; k++) {
        surf_features[i][j][k] = features[i][j].at<float>(0, k);
      }
      cout << "copied descriptor " << i << ',' << j << " size " << surf_features[i][j].size() << endl;
    }
  }
  cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
  voc.create(surf_features);
  //voc.create(features);

  cout << "... done!" << endl;


  // save the vocabulary to disk
  cout << endl << "Saving vocabulary... " << saveto << endl;
  voc.save(saveto);
  cout << "Done" << endl;
}

#include "get_features.cpp"

string image_dir("../sheetimages");
void loadFeatures(const string& images, vector<vector<cv::Mat > > &all_features, const bool firstonly) {

  // add images to the database
  //cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();
  //cv::Ptr<cv::ORB> orb = cv::ORB::create();
  //for(const auto& entry : boost::make_iterator_range(directory_iterator(image_dir), {}))
  int minHessian = 400;
  cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create( minHessian );
  ifstream index(images);
  int last_cluster_id = -1;
  while (!index.eof()) {
    int id, cluster_id;
    string filename;
    index >> id >> filename >> cluster_id;
    if (filename.size() == 0 || (firstonly && last_cluster_id == cluster_id)) continue;
    last_cluster_id = cluster_id;

    cv::Mat image = cv::imread(image_dir + '/' + filename, 0);
    if (image.rows == 0) {
      cout << "bad file " << filename << endl;
      exit(1);
    }
    vector<cv::Mat> features;
    //getSift(f2d, image, features);
    //getOrb(orb, image, features);
    getSurf(surf, image, features);
    all_features.push_back(features);
  }
}

// ----------------------------------------------------------------------------
// ./dictionary MYvoc.txt ../index.txt firstonly
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << endl;
  vector<vector<cv::Mat > > all_features;
  loadFeatures(argv[2], all_features, string(argv[3]) == "firstonly");

  testVocCreation(all_features, argv[1]);
  return 0;
}
