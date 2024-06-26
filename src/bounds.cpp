#include <iostream>

#include "bounds.hpp"

// LP paper

double freq_UB(dist_t& dist, int64_t G, double err) { // Coro 4
  int64_t top_G_freq = most_frequent(dist, G);
  double eps = sqrtl(-log(err) / (2.0 * dist.N));
  return std::min(((double) top_G_freq) / ((double) dist.N) + eps, 1.0);
}

double samp_LB(dist_t& dist, int64_t G, double err) { // Thm 5
  if (dist.D2.empty()) {
    std::cerr << "must partition before calculating sampling LB" << std::endl;
    return -1.0;
  }

  double t = sqrtl(-log(err) / (2.0 * dist.D2.size()));
}

double prior_LB(dist_t& dist, int64_t G, int64_t j, double err1, double err2) { // Thm 9
  if (G <= dist.N) {
    std::cerr << "No L value satisfy the constraints on the parameters" << std::endl;
    return -1.0;
  }

  if (j < 2) {
    std::cerr << "Invalid j value. j must be greater than or equal to 2." << std::endl;
    return -1.0;
  }

  double L = ((double) G) / ((double) dist.N);
  double temp = (double) dist.N;
  for (int i=0; i<j-1; ++i) {
    temp /= ((j-1) * L);
  }

  int lo = 0, hi = dist.freqcount.size() - 1, mid;
  while (lo != hi) {
    mid = (lo + hi + 1) >> 1;
    if (dist.freqcount[mid].first >= j) {
      lo = mid;
    }
    else {
      hi = mid - 1;
    }
  }

  double f_SL = ((double) dist.preftotal[lo]) / ((double) dist.N) - temp;
  double t = sqrtl(-dist.N * j * j * log(err1) / 2);
  double eps = sqrtl(-log(err2) / (2.0 * dist.N));

  return std::max(f_SL - t/dist.N - eps, 0.0);
}

double extended_LB() { // Coro 7
}

// double best_prior_LB(dist_t& dist, int64_t G, double err1, double err2) { // Thm 9 with different j, return best
// }


// PIN paper

double new_LB_model() { // Thm 3
}

double new_LB_samp() { // Coro 4
}

double new_UB(dist_t& dist, int64_t G, double err) { // Thm 2
  int64_t F = most_frequent(dist, G);
}
