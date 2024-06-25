#pragma once

#include <stdint.h>
#include "distribution.hpp"

// LP paper
double freq_UB(dist_t& dist, int64_t G, double err); // Coro 4

double samp_LB(dist_t& dist, int64_t G, int64_t d, double err); // Thm 5

double prior_LB(dist_t& dist, int64_t G, int64_t j, double err1, double err2); // Thm 9

double best_prior_LB(dist_t& dist, int64_t G, double err1, double err2); // Thm 9 with different j, return best

double extended_LB(); // Coro 7


// PIN paper
double new_LB_model(); // Thm 3

double new_LB_samp(); // Coro 4

double new_UB(dist_t& dist, int64_t G, double err); // Thm 2
