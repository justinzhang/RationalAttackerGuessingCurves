#include <iostream>

#include "bounds.hpp"
#include "helpers.hpp"

// LP paper

double freq_UB(dist_t& dist, int64_t G, double err) { // Coro 4
  int64_t top_G_freq = most_frequent(dist, G);
  double eps = sqrtl(-log(err) / (2.0 * dist.N));
  return std::min(((double) top_G_freq) / ((double) dist.N) + eps, 1.0);
}

double samp_LB(dist_t& dist, int64_t G, double err) { // Thm 5
  if (dist.D2_idx.empty()) {
    std::cerr << "must partition before calculating sampling LB" << std::endl;
    return -1.0;
  }
  
  if (dist.D1_attack_hits.size() == 0 || dist.D1_attack_hits[0].first > G) {
    return 0.0;
  }

  int64_t lo = 0, hi = dist.D1_attack_hits.size() - 1, mid;
  while (lo != hi) {
    mid = (lo + hi + 1) >> 1;
    if (dist.D1_attack_hits[mid].first <= G) {
      lo = mid;
    }
    else {
      hi = mid - 1;
    }
  }
  int64_t h_D1_D2_G = dist.D1_attack_hits[lo].second;

  double t = sqrtl(-log(err) / (2.0 * dist.D2_idx.size()));
  return ((double) h_D1_D2_G - t) / dist.D2_idx.size();
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

  if (F == dist.N) {
    return 1.0;
  }

  int64_t iterations = 50;
  double lo=0.0, hi=1.0, mid;

  while (iterations--) {
    mid = (lo + hi) / 2.0;
    double cdf = bcdf(F, dist.N, mid);
    if (cdf > err) {
      lo = mid;
    }
    else {
      hi = mid;
    }
  }

  return mid;
}
