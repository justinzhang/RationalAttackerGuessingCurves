#ifndef DISTRIBUTION
#define DISTRIBUTION

#include <vector>
#include <iostream>
#include <string>

using namespace std;

struct dist_t {
  string filename;
  string filetype;
  vector<pair<int64_t, int64_t>> freqcount;
  vector<int64_t> preftotal;
  vector<int64_t> prefcount;
  int64_t N;
  int64_t distinct;
};

void print(dist_t&);
int64_t most_frequent(dist_t&, int64_t);

#endif // DISTRIBUTION
