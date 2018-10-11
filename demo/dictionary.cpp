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

  OrbVocabulary voc(k, L, weight, score);

  cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
  voc.create(features);
  cout << "... done!" << endl;


  // save the vocabulary to disk
  cout << endl << "Saving vocabulary... " << saveto << endl;
  voc.save(saveto);
  cout << "Done" << endl;
}

// ----------------------------------------------------------------------------

string image_dir("../sheetimages");
void loadFeatures(const string& images, vector<vector<cv::Mat > > &all_features) {

  // add images to the database
  cv::Ptr<cv::ORB> orb = cv::ORB::create();
  //for(const auto& entry : boost::make_iterator_range(directory_iterator(image_dir), {}))
  ifstream index(images);
  while (!index.eof()) {
    int id, cluster_id;
    string filename;
    index >> id >> filename >> cluster_id;

    cv::Mat image = cv::imread(image_dir + '/' + filename, 0);
    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    orb->detectAndCompute(image, mask, keypoints, descriptors);

    vector<cv::Mat> features;
    features.resize(descriptors.rows);
    for(int i = 0; i < descriptors.rows; ++i)
    {
      features[i] = descriptors.row(i);
    }
    all_features.push_back(features);
  }
}

// ----------------------------------------------------------------------------
// ./dictionary MYvoc.txt ../index.txt
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << endl;
  vector<vector<cv::Mat > > all_features;
  loadFeatures(argv[2], all_features);

  testVocCreation(all_features, argv[1]);
  return 0;
}
