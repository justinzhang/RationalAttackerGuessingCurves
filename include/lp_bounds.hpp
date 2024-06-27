#pragma once

#include "gurobi_c++.h"

#include <vector>
#include "distribution.hpp"

double LP_lower(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&, std::string);

double LP_upper(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&, std::string);

double LP_LB(dist_t&, int64_t, double, double, std::vector<double>); // select xhats or not????

double LP_UB(dist_t&, int64_t, double, double, std::vector<double>); // select xhats or not????
