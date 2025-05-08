#pragma once

#include <vector>
#include <map>

#include "distribution.hpp"

double LP_lower(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&);
double LP_upper(dist_t&, int64_t, std::vector<double>&, double, int64_t, int64_t, std::vector<double>&, std::vector<double>&, std::vector<double>&);
double LP_LB(dist_t&, int64_t, double, int64_t, std::vector<double>, std::vector<double>);
double LP_UB(dist_t&, int64_t, double, int64_t, std::vector<double>, std::vector<double>);
double LP_LB_fast(dist_t&, int64_t, double);
double LP_UB_fast(dist_t&, int64_t, double);
double LP_LB_normal(dist_t&, int64_t, double);
double LP_UB_normal(dist_t&, int64_t, double);
double LP_LB_slow(dist_t&, int64_t, double);
double LP_UB_slow(dist_t&, int64_t, double);
double LP_LB(dist_t&, int64_t, double); 
double LP_UB(dist_t&, int64_t, double); 

std::pair<int, double>  threshold_LP(double v, double k, int B, int B_star, 
    std::map<int,std::pair<double,double>> lamBounds, int name);