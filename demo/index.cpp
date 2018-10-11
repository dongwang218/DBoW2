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
#include "DBoW2.h" // defines OrbVocabulary and OrbDatabase

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>


using namespace DBoW2;
using namespace std;
using namespace boost::filesystem;



// ----------------------------------------------------------------------------

OrbVocabulary* testVocCreation(const string& strVocFile)
{
  OrbVocabulary* mpVocabulary = new OrbVocabulary();
  if (strVocFile.find(".txt") != string::npos) mpVocabulary->loadFromTextFile(strVocFile);
  else mpVocabulary->load(strVocFile);
  cout << "Vocabulary loaded!" << endl << endl;
  return mpVocabulary;
}


// ----------------------------------------------------------------------------

string image_dir("../sheetimages");
void testDatabase(const OrbVocabulary& voc, const string& images, const string& db_file) {
  cout << "Creating a small database..." << endl;

  OrbDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

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
    EntryId eid = db.add(features);
    assert(eid == id);
    std::cout << eid << ' ' << filename << '\n';
  }
  // write the id to filename mapping

  cout << "Saving database..." << endl;
  db.save(db_file);
  cout << "... done!" << endl;
}

// ----------------------------------------------------------------------------
// ./index ORBvoc.txt ../index.txt mydb.yml.gz
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << endl;
  OrbVocabulary* voc = testVocCreation(argv[1]);

  testDatabase(*voc, argv[2], argv[3]);

  return 0;
}
