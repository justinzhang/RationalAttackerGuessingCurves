#include "distribution.hpp"
#include <iostream>
#include <unordered_set>
#include <random>
#include <algorithm>

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

  dist.D2.resize(d);
  for (int i=0; i<d; ++i) {
    while (1) {
      int choice = uniform_dist(gen);
      if (appeared.insert(choice).second) {
        dist.D2[i] = choice;
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
    dist.D2[i] = v[i];
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
}

void partition(dist_t& dist, double fraction) {
  if (fraction <= 0 || fraction > 1) {
    std::cerr << "Invalid fraction. Nothing done." << std::endl;
    return;
  }
  else if (fraction < 0.1) {
    partition_small_d(dist, (int64_t) floor(dist.N * fraction));
  }
  else {
    partition_small_d(dist, (int64_t) floor(dist.N * fraction));
  }
}

void hybrid_attack(std::string attack_filename) {
  return;
}

