#include <iostream>

#include "distribution.hpp"
#include "pwdio.hpp"
#include "bounds.hpp"
#include "lp_bounds.hpp"
#include "wrappers.hpp"
#include "plotting.hpp"
#include <fstream>

//yahoo
// N = 69301337

//print statistics of the dataset -- for ease of access when writing this up
int main() {
  // create the dist_t object and reading the sample file
  dist_t yahoo_dist;
  if (read_file(yahoo_dist, "./dataset/yahoo_freqcount.txt", "freqcount")) {
    
    std::cout << "N:" << yahoo_dist.N << std::endl; 

  }
}