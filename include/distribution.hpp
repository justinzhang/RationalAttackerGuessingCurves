#pragma once

#include <vector>
#include <string>

struct dist_t {
  std::string filename;
  std::string filetype;
  std::vector<std::pair<int64_t, int64_t>> freqcount;
  std::vector<int64_t> preftotal;
  std::vector<int64_t> prefcount;
  std::vector<int64_t> D2;
  int64_t N;
  int64_t distinct;
};

void print(dist_t&);
int64_t most_frequent(dist_t&, int64_t);

void partition_small_d(dist_t&, int64_t);
void partition_large_d(dist_t&, int64_t);
void partition(dist_t&, int64_t);
void partition(dist_t&, double);
void hybrid_attack(std::string);

