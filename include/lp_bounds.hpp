#pragma once

#include "gurobi_c++.h"

#include <vector>
#include "distribution.hpp"

double LP_lower(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&);

double LP_upper(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&);

double LP_LB(dist_t&, int64_t, double, int64_t, std::vector<double>, std::vector<double>);

double LP_UB(dist_t&, int64_t, double, int64_t, std::vector<double>, std::vector<double>);

double LP_LB(dist_t&, int64_t, double); // automatically selects parameters

double LP_UB(dist_t&, int64_t, double); // automatically selects parameters
