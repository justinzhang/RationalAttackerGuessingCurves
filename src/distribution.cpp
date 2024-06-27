#include "distribution.hpp"

#include <iostream>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>

void print(dist_t& d) {
  std::cout << "-----------------\n";
  std::cout << "Dataset\n";
  std::cout << "Filename: " << d.filename << "; Filetype:" << d.filetype << '\n';
  std::cout << "Distinct: " << d.distinct << "; Total: " << d.N << '\n';
  std::cout << "freqcount:\n";
  for (auto x:d.freqcount) {
    std::cout << "  (" << x.first << ", " << x.second << ")\n";
  }
  std::cout << "-----------------\n";
}

int64_t most_frequent(dist_t& dist, int64_t G) { // cumulative frequency of top G most frequent passwords
  auto it = std::lower_bound(dist.prefcount.begin(), dist.prefcount.end(), G);
  if (it == dist.prefcount.begin()) {
    return G * dist.freqcount[0].first;
  }
  else if (it == dist.prefcount.end()) {
    return dist.N;
  }
  else {
    int64_t id = it - dist.prefcount.begin();
    return dist.preftotal[id] - (dist.prefcount[id] - G) * dist.freqcount[id].first;
  }
}

void partition_small_d(dist_t& dist, int64_t d) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> uniform_dist(1, dist.N);
  std::unordered_set<int> appeared;

  dist.D2_idx.resize(d);
  for (int i=0; i<d; ++i) {
    while (1) {
      int choice = uniform_dist(gen);
      if (appeared.insert(choice).second) {
        dist.D2_idx[i] = choice;
        break;
      }
    }
  }
}

void partition_large_d(dist_t& dist, int64_t d) {
  std::vector<int64_t> v(dist.N);
  for (int i=0; i<v.size(); ++i) {
    v[i] = i+1;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(v.begin(), v.end(), gen);
  for (int i=0; i<d; ++i) {
    dist.D2_idx[i] = v[i];
  }
}

void partition(dist_t& dist, int64_t d) {
  if (d > dist.N) {
    std::cerr << "Invalid d value: greater than number of samples. Nothing done." << std::endl;
    return;
  }
  if (d <= 0) {
    std::cerr << "Invalid d value: d must be a positive number. Nothing done." << std::endl;
    return;
  }
  else if (d * 10 <= dist.N) {
    partition_small_d(dist, d);
  }
  else {
    partition_large_d(dist, d);
  }
  sort(dist.D2_idx.begin(), dist.D2_idx.end());
}

void partition(dist_t& dist, double fraction) {
  if (fraction <= 0 || fraction > 1) {
    std::cerr << "Invalid fraction. Nothing done." << std::endl;
    return;
  }
  partition(dist, (int64_t) floor(fraction * dist.N));
}

void write_partition_plain(dist_t& dist, std::string D1_filename, std::string D2_filename) {
}

void write_partition_pwdfreq(dist_t& dist, std::string D1_filename, std::string D2_filename) {
}

void write_partition(dist_t& dist, std::string D1_filename, std::string D2_filename, std::string filetype) {
}

void attack_plain(dist_t& dist, std::string attack_filename) {
  std::ifstream fdist(dist.filename);
  if (!fdist.is_open()) {
    std::cerr << "Error: Can't open the file associated with the distribution. Nothing done." << std::endl;
    return;
  }
  std::string pwd;
  std::unordered_map<std::string, int64_t> hist;

  for (int i=1, j=0; i<=dist.N; ++i) {
    if (!getline(fdist, pwd)) {
      std::cerr << "Error: error while reading from the file associated with the distribution. ";
      std::cerr << "Distribution file should contain " << dist.N << " lines. Nothing done." << std::endl;
      return;
    }
    if (i == dist.D2_idx[j]) {
      ++j;
      hist[pwd]++;
    }
  }

  std::ifstream fattk(attack_filename);
  if (!fattk.is_open()) {
    std::cerr << "Error: Can't open the attack file. Nothing done." << std::endl;
    return;
  }

  dist.attack_hits.clear();
  dist.attack_hits.push_back(0);
  while (getline(fattk, pwd)) {
    dist.attack_hits.push_back(1);
  }
}

void attack_pwdfreq(dist_t& dist, std::string attack_filename) {
  std::ifstream fdist(dist.filename);
  if (!fdist.is_open()) {
    std::cerr << "Error: Can't open the file associated with the distribution. Nothing done." << std::endl;
    return;
  }
  std::ifstream fattk(attack_filename);
  if (!fattk.is_open()) {
    std::cerr << "Error: Can't open the attack file. Nothing done." << std::endl;
    return;
  }
}

void hybrid_attack(dist_t& dist, std::string attack_filename) {
  if (dist.filetype == "freqcount") {
    std::cerr << "Error: Can't attack a frequency-count file. Please specify a plaintext or password-frequency file. Nothing done." << std::endl;
  }
  else if (dist.filetype == "plain") {
    attack_plain(dist, attack_filename);
  }
  else if (dist.filetype == "pwdfreq") {
    attack_pwdfreq(dist, attack_filename);
  }
  else {
    std::cerr << "Error: Invalid filetype. Nothing done." << std::endl;
  }
}

