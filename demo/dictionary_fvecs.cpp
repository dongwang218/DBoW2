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
template <class T>
T get_value(unsigned char *buf) {
  unsigned char temp[sizeof(T)];
  for (int i = 0, j= sizeof(T)-1; i < sizeof(T); ++i, --j) {
    temp[i] = buf[i];
  }
  return reinterpret_cast<T&>(temp);
}

std::string hexStr(unsigned char *data, int len)
{
    std::stringstream ss;
    ss<<std::hex;
    for(int i(0);i<len;++i)
        ss<<(int)data[i];
    return ss.str();
}

void read_fvecs(const string fvecs, vector<vector<float>>& sift) {
  ifstream my_file(fvecs, ios::in | ios::binary);
  long fsize = my_file.tellg();
  my_file.seekg( 0, std::ios::end );
  fsize = my_file.tellg() - fsize;
  cout << "Reading " << fvecs << " total " << fsize << " bytes" << endl;
  my_file.seekg( 0, std::ios::beg );

  // temp
  unsigned char *buf = new unsigned char[fsize];
  my_file.read(reinterpret_cast<char*>(buf), fsize);
  //cout << hexStr(buf, 4) << endl;

  unsigned char* temp = buf;
  int d;
  d = get_value<int>(temp);
  cout << "Dim " << d << endl;

  int vecsizeof = 4 + d * 4;
  int total = fsize / vecsizeof;

  sift.clear();
  sift.resize(total);
  float f;
  for (int i = 0; i < total; i++) {
    cout << i << " out of " << total << endl;
    d = get_value<int>(temp);
    temp += sizeof(int);
    //cout << endl << d << endl;
    sift[i].resize(d);
    for (int j = 0; j < d; ++j) {
      f = get_value<float>(temp);
      temp += sizeof(int);
      sift[i][j] = f;
      //cout << f << ',';
    }
  }
  delete[] buf;
}

void testVocCreation(const vector<vector<float >> &features, const string& saveto)
{
  // branching factor and depth levels
  const int k = 10;
  const int L = 6;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  //OrbVocabulary voc(k, L, weight, score);
  SiftVocabulary voc(k, L, weight, score);

  // conver to vector<vector<float>>
  cout << "Creating a " << k << "^" << L << " vocabulary..." << endl;
  voc.create({features});
  cout << "... done!" << endl;


  // save the vocabulary to disk
  cout << endl << "Saving vocabulary... " << saveto << endl;
  voc.save(saveto);
  cout << "Done" << endl;
}

// ----------------------------------------------------------------------------
// ./dictionary_fvecs SIFT1M.yml.gz sift/sift_base.fvecs
int main(int argc, char** argv)
{
  cout << "Running " << argv[0] << ' ' << argv[1] << ' ' << argv[2] << endl;
  vector<vector<float>> sift;
  read_fvecs(string(argv[2]), sift);

  testVocCreation(sift, argv[1]);
  return 0;
}
