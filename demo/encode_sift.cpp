/**
 * File: Demo.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DBoW2
 * License: see the LICENSE.txt file
 */

#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

// DBoW2
#include "DBoW2.h" // defines SiftVocabulary and SiftDatabase

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "opencv2/xfeatures2d.hpp"

using namespace DBoW2;
using namespace std;
using namespace boost::filesystem;



// ----------------------------------------------------------------------------

SiftVocabulary* testVocCreation(const string& strVocFile)
{
  SiftVocabulary* mpVocabulary = new SiftVocabulary();
  if (strVocFile.find(".txt") != string::npos) mpVocabulary->loadFromTextFile(strVocFile);
  else mpVocabulary->load(strVocFile);
  cout << "Vocabulary loaded!" << endl << endl;
  return mpVocabulary;
}

// ----------------------------------------------------------------------------
// ./encode_sift voc.yml.gz feature.yml
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << endl;
  int topn = std::stoi(argv[3]);
  SiftVocabulary* voc = testVocCreation(argv[1]);

  cout << "Size of voc " << voc->size() << endl;

  cv::FileStorage fs(argv[2], cv::FileStorage::READ);
  cv::Mat sift;
  fs["features"] >> sift;
  vector<vector<float>> sift_features(sift.rows);

  for (int i = 0; i < sift.rows; ++i) {
    sift_features[i].resize(sift.cols);
    for (int j = 0; j < sift.cols; ++j) {
      sift_features[i][j] =  sift.at<float>(i, j);
    }
  }

  BowVector vec;
  vector<pair<double, int>> sorted;
  voc->transform(sift_features, vec);
  // sort by weight and take the top 100
  //vector<bool> bitvec(voc->size());
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    //cout << it->first << ',';
    //bitvec[it->first] = true;
    sorted.push_back({it->second, it->first});
  }
  sort(sorted.begin(), sorted.end(), [](const pair<float, int>& one, const pair<float, int>& two){
      return two.first == one.first ? one.second < two.second : one.first > two.first;
    });
  cout << "most weighted " << sorted[0].first << " least weighted " << sorted[sorted.size()-1].first << endl;
  vector<int> sorted_index;
  for (int j = 0; j < topn && j < sorted.size(); ++j) {
    //cout << sorted[j].second << ",";
    sorted_index.push_back(sorted[j].second);
  }
  sort(sorted_index.begin(), sorted_index.end());
  for (int j = 1; j < sorted_index.size(); ++j) {
    sorted_index[j] -= sorted_index[j-1];
  }
  for (int j = 0; j < sorted_index.size(); ++j) {
    cout << sorted_index[j] << ",";
  }
  cout << endl;
  // convert to digits
  /*string r;
  for (int i = 0; i < vec.size(); i += 3) {
    int sum = 0;
    for (int j = i; j < vec.size() && j < i+3; ++j) {
      sum *= 2;
      if (vec[j]) sum++;
    }
    r += std::to_string(sum);
  }
  cout << r << endl;*/
  return 0;
}
