#include "pwdio.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>

#include "distribution.hpp"

void parse_freqcount(dist_t& dist, std::vector<std::pair<int64_t, int64_t>>& freqcount) {
  std::sort(freqcount.rbegin(), freqcount.rend()); // sort descending

  std::vector<int64_t> preftotal(freqcount.size(), 0);
  std::vector<int64_t> prefcount(freqcount.size(), 0);

  preftotal[0] = freqcount[0].first * freqcount[0].second;
  prefcount[0] = freqcount[0].second;

  for (int i=1; i<prefcount.size(); ++i) {
    preftotal[i] = preftotal[i-1] + freqcount[i].first * freqcount[i].second;
    prefcount[i] = prefcount[i-1] + freqcount[i].second;
  }

  dist.freqcount = freqcount;
  dist.preftotal = preftotal;
  dist.prefcount = prefcount;
  dist.N = preftotal.back();
  dist.distinct = prefcount.back();
}

void read_plain(dist_t& dist, std::string filename) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    std::cerr << "Error: can't open file " << filename << std::endl;
    return;
  }

  std::unordered_map<std::string, int64_t> hist;
  std::unordered_map<int64_t, int64_t> cnt;

  dist.filename = filename;
  dist.filetype = "plain";

  std::string pwd;
  while (std::getline(fin, pwd)) {
    hist[pwd]++;
  }
  fin.close();

  for (auto& it : hist) {
    cnt[it.second]++;
  }
  std::vector<std::pair<int64_t, int64_t>> freqcount;
  for (auto& it : cnt) {
    freqcount.push_back({it.first, it.second});
  }
  parse_freqcount(dist, freqcount);
}

void read_pwdfreq(dist_t& dist, std::string filename) { // pwd freq seperated with \t
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    std::cerr << "Error: can't open file " << filename << std::endl;
    return;
  }

  dist.filename = filename;
  dist.filetype = "pwdfreq";

  std::unordered_map<int64_t, int64_t> cnt;
  std::string pwd, line;
  int64_t freq;
  while (std::getline(fin, line)) {
    std::istringstream ss(line);
    if (std::getline(ss, pwd, '\t') && ss >> freq) {
      cnt[freq]++;
    }
    else {
      std::cerr << "Error: Invalid line: " << line << " in file " << filename << std::endl;
    }
  }
  fin.close();

  std::vector<std::pair<int64_t, int64_t>> freqcount;
  for (auto& it : cnt) {
    freqcount.push_back({it.first, it.second});
  }
  parse_freqcount(dist, freqcount);
}

void read_freqcount(dist_t& dist, std::string filename) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    std::cerr << "Error: can't open file " << filename << std::endl;
    return;
  }

  dist.filename = filename;
  dist.filetype = "freqcount";

  int64_t freq, count;
  std::string line;
  std::vector<std::pair<int64_t, int64_t>> freqcount;

  while (std::getline(fin, line)) {
    std::istringstream ss(line);
    if (ss >> freq >> count && ss.peek() == std::char_traits<char>::eof()) {
      freqcount.push_back({freq, count});
    }
    else {
      std::cerr << "invalid line: " << line << std::endl;
    }
  }
  fin.close();

  parse_freqcount(dist, freqcount);
}

void write_freqcount(dist_t& dist, std::string filename) { // each line is (freq count)
  std::ofstream fout(filename);
  if (!fout.is_open()) {
    std::cerr << "Error: can't open file " << filename << std::endl;
  }

  for (auto x : dist.freqcount) {
    fout << x.first << ' ' << x.second << '\n';
  }
  fout.close();
}

dist_t create(std::string filename, std::string filetype) {
  dist_t res;
  if (filetype == "plain") {
    read_plain(res, filename);
  }
  else if (filetype == "pwdfreq") {
    read_pwdfreq(res, filename);
  }
  else if (filetype == "freqcount") {
    read_freqcount(res, filename);
  }
  else {
    std::cerr << "Error: " << filetype << " is not a valid filetype" << std::endl;
  }
  return res;
}


