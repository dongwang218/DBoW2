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

#include "get_features.cpp"

// ----------------------------------------------------------------------------

string image_dir("../sheetimages");
void testDatabase(const SiftVocabulary& voc, const string& images, const string& db_file, bool firstonly) {
  cout << "Creating a small database..." << endl;

  SiftDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  int minHessian = 4000;
  // add images to the database
  //cv::Ptr<cv::ORB> orb = cv::ORB::create();
  cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create( minHessian );

  //for(const auto& entry : boost::make_iterator_range(directory_iterator(image_dir), {}))
  ifstream index(images);
  int last_cluster_id = -1;
  while (!index.eof()) {
    int id, cluster_id;
    string filename;
    index >> id >> filename >> cluster_id;
    if (filename.size() == 0 || (firstonly && last_cluster_id == cluster_id)) continue;
    last_cluster_id = cluster_id;

    cv::Mat image = cv::imread(image_dir + '/' + filename, 0);
    vector<cv::Mat> features;
    //getSift(f2d, image, features);
    getSift(sift, image, features);
    vector<vector<float>> sift_features(features.size());

    for (int i = 0; i < features.size(); ++i) {
      mat2vector(features[i], sift_features[i]);
    }

    EntryId eid = db.add(sift_features);
    assert(eid == id || eid == cluster_id);
    std::cout << eid << ' ' << filename << '\n';
  }
  // write the id to filename mapping

  cout << "Saving database..." << endl;
  db.save(db_file);
  cout << "... done!" << endl;
}

// ----------------------------------------------------------------------------
// ./index ORBvoc.txt ../index.txt mydb.yml.gz firstonly
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << endl;
  SiftVocabulary* voc = testVocCreation(argv[1]);

  testDatabase(*voc, argv[2], argv[3], string(argv[4]) == "firstonly");

  return 0;
}
