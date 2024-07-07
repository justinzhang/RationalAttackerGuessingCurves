#pragma once

#include <stdint.h>

#include "distribution.hpp"

// LP paper
double freq_UB(dist_t&, int64_t, double); // Coro 4
double samp_LB(dist_t&, int64_t, double); // Thm 5
double extended_LB(dist_t&, int64_t, double); // Coro 7
double prior_LB(dist_t&, int64_t, int64_t, double, double); // Thm 9
double prior_LB(dist_t&, int64_t, int64_t, double); // Thm 9
double best_prior_LB(dist_t&, int64_t, double, double); // Thm 9
double best_prior_LB(dist_t&, int64_t, double); // Thm 9, automatically selects parameters

// PIN paper
// double new_LB_model(); // Thm 3
double new_LB_samp(dist_t& dist, int64_t G, double err); // Coro 4
double new_UB(dist_t& dist, int64_t G, double err); // Thm 2
