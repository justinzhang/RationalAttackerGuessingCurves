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

void count_in_partition(dist_t& dist, std::unordered_map<std::string, int64_t>& hist_D1, std::unordered_map<std::string, int64_t>& hist_D2) {
  std::ifstream fin(dist.filename);
  if (!fin.is_open()) {
    std::cerr << "Error: can't open file " << dist.filename << ". Nothing done." << std::endl;
    return;
  }
  std::string pwd;
  std::string line;

  if (dist.filetype == "plain") {
    int64_t i = 0;
    int64_t cnt = 1;
    while (getline(fin, pwd)) {
      if (i <= dist.D2_idx.size() && cnt == dist.D2_idx[i]) {
        hist_D2[pwd]++;
        ++i;
      }
      else {
        hist_D1[pwd]++;
      }
      ++cnt;
    }
  }
  else {
    int64_t freq;
    int64_t cumulative_freq = 0;
    int64_t i = 0;
    while (std::getline(fin, line)) {
      std::istringstream ss(line);
      if (std::getline(ss, pwd, '\t') && ss >> freq) {
        cumulative_freq += freq;
        int64_t in_D2_cnt = 0;
        while (i < dist.D2_idx.size() && dist.D2_idx[i] <= cumulative_freq) {
          ++i;
          ++in_D2_cnt;
        }
        hist_D1[pwd] += (freq - in_D2_cnt);
        hist_D2[pwd] += in_D2_cnt;
      }
      else {
        std::cerr << "Error: Invalid line: " << line << " in file " << dist.filename << std::endl;
      }
    }
  }

  fin.close();
}

void write_partition(dist_t& dist, std::unordered_map<std::string, int64_t>& hist_D1, std::unordered_map<std::string, int64_t>& hist_D2, std::string D1_filename, std::string D2_filename, std::string filetype) {
  if (D1_filename.size() > 0) {
    std::ofstream fout(D1_filename);
    if (!fout.is_open()) {
      std::cerr << "Error: Can't open file " << D1_filename << std::endl;
    }
    else {
      if (filetype == "plain") {
        for (auto& x:hist_D1) {
          for (int i=0; i<x.second; ++i) {
            fout << x.first << '\n';
          }
        }
      }
      else {
        for (auto& x:hist_D1) {
          fout << x.first << '\t' << x.second << '\n';
        }
      }
      fout.close();
    }
  }

  if (D2_filename.size() > 0) {
    std::ofstream fout(D2_filename);
    if (!fout.is_open()) {
      std::cerr << "Error: Can't open file " << D2_filename << std::endl;
    }
    else {
      if (filetype == "plain") {
        for (auto& x:hist_D2) {
          for (int i=0; i<x.second; ++i) {
            fout << x.first << '\n';
          }
        }
      }
      else {
        for (auto& x:hist_D2) {
          fout << x.first << '\t' << x.second << '\n';
        }
      }
      fout.close();
    }
  }
}

void partition(dist_t& dist, int64_t d, std::string D1_filename, std::string D2_filename, std::string filetype="plain") {
  if (d > dist.N) {
    std::cerr << "Error: Invalid d value " << d << " is greater than number of samples " << dist.N << ". Nothing done." << std::endl;
    return;
  }

  if (d <= 0) {
    std::cerr << "Error: Invalid d value " << d << ". d must be a positive number. Nothing done." << std::endl;
    return;
  }

  if (filetype != "plain" && filetype != "pwdfreq") {
    std::cerr << "Error: Invalid filetype " << filetype << ". Must be either \"plain\" or \"pwdfreq\". Nothing done." << std::endl; 
    return;
  }

  if (d * 10 <= dist.N) {
    partition_small_d(dist, d);
  }
  else {
    partition_large_d(dist, d);
  }
  sort(dist.D2_idx.begin(), dist.D2_idx.end());

  std::unordered_map<std::string, int64_t> D1_hist;
  std::unordered_map<std::string, int64_t> D2_hist;
  dist.D2_hist = D2_hist;
  count_in_partition(dist, D1_hist, D2_hist);
  write_partition(dist, D1_hist, D2_hist, D1_filename, D2_filename, filetype);

  // precompute dictionary attack with D1
  std::vector<std::pair<std::string, int64_t>> D1_pwdfreq;
  for (auto& x:D1_hist) {
    D1_pwdfreq.push_back(std::make_pair(x.first, x.second));
  }
  sort(D1_pwdfreq.begin(), D1_pwdfreq.end(), [&](std::pair<std::string, int64_t>& a, std::pair<std::string, int64_t>& b) {
    if (a.second == b.second) {
      return D2_hist[a.first] > D2_hist[b.first];
    }
    return a.second > b.second;
  });

  int64_t cur_hits = 0;
  dist.D1_attack_hits.clear();
  for (int i=0; i<D1_pwdfreq.size() && cur_hits<d; ++i) {
    if (D2_hist[D1_pwdfreq[i].first] != 0) {
      cur_hits += D2_hist[D1_pwdfreq[i].first];
      dist.D1_attack_hits.push_back(std::make_pair(i+1, cur_hits));
    }
  }
}

void partition(dist_t& dist, double fraction, std::string D1_filename, std::string D2_filename, std::string filetype) {
  if (fraction <= 0 || fraction > 1) {
    std::cerr << "Error: Invalid fraction " << fraction << ". Nothing done." << std::endl;
    return;
  }
  partition(dist, (int64_t) floor(fraction * dist.N), D1_filename, D2_filename, filetype="plain");
}


// void attack_plain(dist_t& dist, std::string attack_filename) {
//   std::ifstream fdist(dist.filename);
//   if (!fdist.is_open()) {
//     std::cerr << "Error: Can't open the file associated with the distribution. Nothing done." << std::endl;
//     return;
//   }
//   std::string pwd;
//   std::unordered_map<std::string, int64_t> hist;
//
//   for (int i=1, j=0; i<=dist.N; ++i) {
//     if (!getline(fdist, pwd)) {
//       std::cerr << "Error: error while reading from the file associated with the distribution. ";
//       std::cerr << "Distribution file should contain " << dist.N << " lines. Nothing done." << std::endl;
//       return;
//     }
//     if (i == dist.D2_idx[j]) {
//       ++j;
//       hist[pwd]++;
//     }
//   }
//   fdist.close();
//
//   std::ifstream fattk(attack_filename);
//   if (!fattk.is_open()) {
//     std::cerr << "Error: Can't open the attack file. Nothing done." << std::endl;
//     return;
//   }
//
//   dist.attack_hits.clear();
//   dist.attack_hits.push_back(0);
//   while (getline(fattk, pwd)) {
//     dist.attack_hits.push_back(1);
//   }
//   
//   fattk.close();
// }
//
// void attack_pwdfreq(dist_t& dist, std::string attack_filename) {
//   std::ifstream fdist(dist.filename);
//   if (!fdist.is_open()) {
//     std::cerr << "Error: Can't open the file associated with the distribution. Nothing done." << std::endl;
//     return;
//   }
//   std::ifstream fattk(attack_filename);
//   if (!fattk.is_open()) {
//     std::cerr << "Error: Can't open the attack file. Nothing done." << std::endl;
//     return;
//   }
// }
//
// void hybrid_attack(dist_t& dist, std::string attack_filename) {
//   if (dist.filetype == "freqcount") {
//     std::cerr << "Error: Can't attack a frequency-count file. Please specify a plaintext or password-frequency file. Nothing done." << std::endl;
//   }
//   else if (dist.filetype == "plain") {
//     attack_plain(dist, attack_filename);
//   }
//   else if (dist.filetype == "pwdfreq") {
//     attack_pwdfreq(dist, attack_filename);
//   }
//   else {
//     std::cerr << "Error: Invalid filetype. Nothing done." << std::endl;
//   }
// }

