#pragma once

#include <vector>

#include "distribution.hpp"

void parse_freqcount(dist_t&, std::vector<std::pair<int64_t, int64_t>>&);
void read_plain(dist_t&, std::string);
void read_pwdfreq(dist_t&, std::string); // pwd freq seperated with \t
void read_freqcount(dist_t&, std::string);
// other formats ??
void write_freqcount(dist_t&, std::string);
dist_t create(std::string, std::string);
