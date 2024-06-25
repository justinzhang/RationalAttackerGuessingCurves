#ifndef PWDIO
#define PWDIO

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "distribution.hpp"

using namespace std;

void parse_freqcount(dist_t&, vector<pair<int64_t, int64_t>>&);
void read_plain(dist_t&, string);
void read_pwdfreq(dist_t&, string); // pwd freq seperated with \t
void read_freqcount(dist_t&, string);
// other formats ??
void write_freqcount(dist_t&, string);
dist_t create(string, string);

#endif // PWDIO
