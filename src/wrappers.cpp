#include "wrappers.hpp"

#include <iostream>

#include "bounds.hpp"
#include "lp_bounds.hpp"
#include "error_check.hpp"

double best_bound(dist_t& dist, int64_t G, double err) {
}

double best_bound(dist_t& dist, std::vector<int64_t> Gs, double err) {
}

std::unordered_map<std::string, double> bound(dist_t& dist, int64_t G, double err) {
  std::unordered_map<std::string, double> res;

  if (!error_check_basic(dist, G, err)) {
    return res;
  }

  if (dist.D2_idx.size() > 0) res["samp LB"] = samp_LB(dist, G, err);
  if (dist.model_attack_hits.size() > 0) res["extended LB"] = extended_LB(dist, G, err);
  res["freq UB"] = freq_UB(dist, G, err);
  res["LP LB"] = LP_LB(dist, G, err);
  res["LP UB"] = LP_UB(dist, G, err);
  res["binom LB"] = binom_LB(dist, G, err);
  res["binom UB"] = binom_UB(dist, G, err);

  return res;
}

std::unordered_map<std::string, std::vector<double>> bound(dist_t& dist, std::vector<int64_t> Gs, double err) {
  std::unordered_map<std::string, std::vector<double>> res;

  if (!error_check_basic(dist, Gs, err)) {
    return res;
  }

  for (auto G:Gs) {
    if (dist.D2_idx.size() > 0) res["samp LB"].push_back(samp_LB(dist, G, err));
    if (dist.model_attack_hits.size() > 0) res["extended LB"].push_back(extended_LB(dist, G, err));
    res["freq UB"].push_back(freq_UB(dist, G, err));
    res["LP LB"].push_back(LP_LB(dist, G, err));
    res["LP UB"].push_back(LP_UB(dist, G, err));
    res["binom LB"].push_back(binom_LB(dist, G, err));
    res["binom UB"].push_back(binom_UB(dist, G, err));
  }

  return res;
}

std::unordered_map<std::string, double> bound(dist_t& dist, int64_t G, double err, std::vector<std::string> bounds) {
  std::vector<std::string> valid_bound_names = {"samp LB", "extended LB", "freq UB", "LP LB", "LP UB", "binom LB", "binom UB"};
}

std::unordered_map<std::string, std::vector<double>> bound(dist_t& dist, std::vector<int64_t> Gs, double err, std::vector<std::string> bounds) {
}

