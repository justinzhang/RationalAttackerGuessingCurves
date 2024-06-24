#ifndef PWDIO
#define PWDIO

#include <bits/stdc++.h>
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

void print(dist_t& d);

void parse_freqcount(dist_t&, vector<pair<int64_t, int64_t>>&);
void read_plain(dist_t&, string);
void read_pwdfreq(dist_t&, string); // pwd freq seperated with \t
void read_freqcount(dist_t&, string);
// other formats ??
void write_freqcount(dist_t&, string);
dist_t create(string, string);

#endif // PWDIO
