#include <iostream>

#include "bounds.hpp"

// LP paper

double freq_UB(dist_t& dist, int64_t G, double err) { // Coro 4
  int64_t top_G_freq = most_frequent(dist, G);
  double eps = sqrtl(-log(err) / (2.0 * dist.N));
  return min(((double) top_G_freq) / ((double) dist.N) + eps, 1.0);
}

double samp_LB(dist_t& dist, int64_t G, int64_t d, double err) { // Thm 5
}

double prior_LB(dist_t& dist, int64_t G, int64_t j, double err1, double err2) { // Thm 9
  if (G <= dist.N) {
    std::cerr << "No L value satisfy the constraints on the parameters" << std::endl;
    return -1.0;
  }
}

double best_prior_LB(dist_t& dist, int64_t G, double err1, double err2) { // Thm 9 with different j, return best
}

double extended_LB() { // Coro 7
}


// PIN paper
double new_LB_model() { // Thm 3
}

double new_LB_samp() { // Coro 4
}

double new_UB(dist_t& dist, int64_t G, double err) { // Thm 2
  // auto it = lower_bound();
  int64_t F = 0;
}


int main() {
}
