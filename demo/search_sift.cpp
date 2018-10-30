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
#include "DBoW2.h" // defines SiftVocabulary and SiftDatabase

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "opencv2/xfeatures2d.hpp"

using namespace DBoW2;
using namespace std;

#include "get_features.cpp"

// ----------------------------------------------------------------------------
string image_dir("../sheetimages");
void testDatabase(const string& images, const string& db_file, bool firstonly) {
  cout << "Creating a small database..." << endl;

  SiftDatabase db(db_file);
  cout << "total in db " << db.size() << endl;

  int total = 0, correct = 0;

  vector<pair<string, int>> clusters;
  vector<pair<string, int>> results;
  ifstream index(images);
  int last_cluster_id = -1;
  while (!index.eof()) {
    int id, cluster_id;
    string filename;
    index >> id >> filename >> cluster_id;
    if (filename.size() == 0) continue;
    assert(results.size() == id);
    results.push_back({filename, cluster_id});
    if (cluster_id != last_cluster_id) clusters.push_back({filename, cluster_id});
    last_cluster_id = cluster_id;
  }

  int minHessian = 4000;
  //cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();
  //cv::Ptr<cv::ORB> orb = cv::ORB::create();
  cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create( minHessian );
  for (int id = 0; id < results.size(); ++id) {
    cout << "Test " << results[id].first << ' ' << results[id].second << endl;
    int cluster_id;
    string filename;
    filename = results[id].first;
    cluster_id = results[id].second;

    cv::Mat image = cv::imread(image_dir + '/' + filename, 0);
    vector<cv::Mat> features;
    //getSift(f2d, image, features);
    getSift(sift, image, features);
    vector<vector<float>> sift_features(features.size());

    for (int i = 0; i < features.size(); ++i) {
      mat2vector(features[i], sift_features[i]);
    }

    QueryResults ret;
    db.query(sift_features, ret, 6);
    total += firstonly ? 1 : ret.size();
    for (auto& qr: ret) {
      if (firstonly) {
        cout << "  " << qr.Score << ' ' << clusters[qr.Id].first << ' ' << clusters[qr.Id].second << endl;
        if (cluster_id == clusters[qr.Id].second) {
          correct++;
        }
      } else {
        cout << "  " << qr.Score << ' ' << results[qr.Id].first << ' ' << results[qr.Id].second << endl;
        if (cluster_id == results[qr.Id].second) {
          correct++;
        }
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

  testDatabase(argv[1], argv[2], string(argv[3]) == "firstonly");

  return 0;
}
