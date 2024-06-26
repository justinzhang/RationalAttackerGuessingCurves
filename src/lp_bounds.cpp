#include "lp_bounds.hpp"

double LP_lower(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats) {
  return 0.0;
}

double LP_upper(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats) {
  return 0.0;
}

double LP_LB(dist_t& dist, int64_t G, double q, double iprime, std::vector<double> errs) { // select xhats or not????
  // errs is of size iprime+1
  return 0.0;
}

double LP_UB(dist_t& dist, int64_t G, double q, double iprime, std::vector<double> errs) { // select xhats or not????
  // errs is of size iprime+1
  return 0.0;
}
