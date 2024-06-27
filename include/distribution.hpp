#pragma once

#include <vector>
#include <string>

struct dist_t {
  std::string filename;
  std::string filetype;

  std::vector<std::pair<int64_t, int64_t>> freqcount;
  std::vector<int64_t> preftotal;
  std::vector<int64_t> prefcount;

  std::vector<int64_t> D2_idx;
  std::vector<int64_t> D2_pwd;
  std::vector<int64_t> attack_hits;

  int64_t N;
  int64_t distinct;
};

void print(dist_t&);
int64_t most_frequent(dist_t&, int64_t);

void partition_small_d(dist_t&, int64_t);
void partition_large_d(dist_t&, int64_t);
void partition(dist_t&, int64_t);
void partition(dist_t&, double);

void write_partition_plain(dist_t&, std::string, std::string);
void write_partition_pwdfreq(dist_t&, std::string, std::string);
void write_partition(dist_t&, std::string, std::string, std::string);

void attack_plain(dist_t&, std::string);
void attack_pwdfreq(dist_t&, std::string);
void hybrid_attack(dist_t&, std::string);
