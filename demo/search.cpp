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
string image_dir("../sheetimages");
void testDatabase(const string& images, const string& db_file) {
  cout << "Creating a small database..." << endl;

  OrbDatabase db(db_file);

  int total = 0, correct = 0;

  vector<pair<string, int>> results;
  ifstream index(images);
  while (!index.eof()) {
    int id, cluster_id;
    string filename;
    index >> id >> filename >> cluster_id;
    assert(results.size() == id);
    results.push_back({filename, cluster_id});
  }

  cv::Ptr<cv::ORB> orb = cv::ORB::create();
  for (int id = 0; id < results.size(); ++id) {
    cout << "Test " << results[id].first << ' ' << results[id].second << endl;
    int cluster_id;
    string filename;
    filename = results[id].first;
    cluster_id = results[id].second;

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

    QueryResults ret;
    db.query(features, ret, 6);
    total += ret.size();
    for (auto& qr: ret) {
      cout << "  " << qr.Score << ' ' << results[qr.Id].first << ' ' << results[qr.Id].second << endl;
      if (cluster_id == results[qr.Id].second) {
        correct++;
      } else {
      }
    }
  }
  cout << "Total " << total << " Correct " << correct << endl;
}

// ----------------------------------------------------------------------------
// ./search ../index.txt mydb.yml.gz
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << endl;

  testDatabase(argv[1], argv[2]);

  return 0;
}
