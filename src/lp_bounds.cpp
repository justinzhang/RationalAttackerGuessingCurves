#include "lp_bounds.hpp"

#include <iostream>
#include <unordered_map>
#include <cmath>
#include <numeric>

#include "gurobi_c++.h"

#include "helpers.hpp"
#include "error_check.hpp"

std::pair<int, double>  threshold_LP(double v, double k, int B, int B_star, 
  std::map<int,std::pair<double,double>> lamBounds, int name) {
  try {
    GRBEnv env = GRBEnv(true);
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();
    GRBModel model = GRBModel(env);

    // variables -- vars[i] = x_{i + 1}
    std::vector<GRBVar> vars(B_star); 
    std::cout << "there are " << std::to_string(B_star) << std::endl;
    for (int i = 0; i < B_star; i++) {
      vars[i] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "vars[" + std::to_string(i)+ "]");
    }

    // objective -- max/min lambda_B' = \sum_{i = 1}^B x_i = \sum_{i = 0}^{B - 1} vars[i]
    GRBLinExpr obj = 0.0;
    for (int i = 0; i < B; i++) { 
      obj += vars[i];
    }
    
    model.setObjective(obj, name);
    
    // constraints
    // c1: sum to 1
    GRBLinExpr sum_vars = 0.0;
    for (int j = 0; j < B_star; j++) {
      sum_vars += vars[j];
    }
    model.addConstr(1.0, GRB_GREATER_EQUAL, sum_vars, "sum leq 1"); //NOTE: leq 1 by assumption

    // ordered greatest to least
    for (int j = 0; j < B_star - 1; j++) {
      model.addConstr(vars[j], GRB_GREATER_EQUAL,vars[j+1], "greatest to least" + std::to_string(j));
    }


    //util constr 
    std::vector<GRBLinExpr> util(B_star); 

    for (int i = 0; i < B_star; i++) {

      GRBLinExpr curr = 0.0;

      GRBLinExpr lambdaI = 0.0;

      for (int j = 0; j < i; j++) {
        lambdaI += vars[j];
      }

      

      GRBLinExpr iLambdaI = 0.0;
      for (int j = 0; j < i; j++) {
        iLambdaI += (j + 1) * vars[j];
      }

      curr += (lambdaI) * v - (k * iLambdaI + ( 1 - lambdaI) * i * k);
      util[i] = curr;
    }

    for (int j = 0; j < B_star; j++){
      model.addConstr(util[B], GRB_GREATER_EQUAL, util[j], "util " + std::to_string(j));
    }

    int lastB;
    //Lambda constraints 
    for (auto entry : lamBounds) {
        int currB = entry.first;
        double minLam = entry.second.first;
        double maxLam = entry.second.second; 

        if (currB >= vars.size()) {
          lastB = currB;
          break;
        }
        // std::cout << "adding B = " << std::to_string(currB) << " with " << std::to_string(minLam) << "," << std::to_string(maxLam) << std::endl;
        GRBLinExpr lambdaB = 0.0;
        for (int i = 0; i < currB; i++) {
          lambdaB += vars[i];
        }
        model.addConstr(lambdaB, GRB_GREATER_EQUAL, minLam,  std::to_string(currB) + ", minLam " + std::to_string(minLam));
        model.addConstr(lambdaB, GRB_LESS_EQUAL, maxLam, std::to_string(currB) + ", maxLam " + std::to_string(maxLam) );
        
    }


    std::cout << "constraints added successfully up to the bounds prior to B = " << std::to_string(lastB) << std::endl;
    model.optimize();

    // status/solution
    int status = model.get(GRB_IntAttr_Status);
    GRBVar *varReturn = model.getVars();
    // double *values = model.get(GRB_DoubleAttr_X, varReturn, B_star);

		if (status == GRB_OPTIMAL) {
      return std::make_pair(0, model.get(GRB_DoubleAttr_ObjVal));
		}
    else if (status == GRB_INFEASIBLE) {
      std::cout << "infeasible" << std::endl;
      return std::make_pair(-2,-0.0);
    }
    else {
      std::cout << "idk" << std::endl;
      return std::make_pair(-1,-0.0);
    }

  } catch(GRBException e) {
    std::cout << "exception" << std::endl;
    // if (dist.verbose) {
    //   std::cerr << "\n[Error: code = " << e.getErrorCode() << "; message: " << e.getMessage() << ".]" << std::endl;
    // }
    std::cerr << "\n[Error: code = " << e.getErrorCode() << "; message: " << e.getMessage() << ".]" << std::endl;
    return std::make_pair(-3,-0.0);
  } catch(...) {
    std::cerr << "\n[Error: Exception during optimization.]" << std::endl;
    // if (dist.verbose) {
      // std::cerr << "\n[Error: Exception during optimization.]" << std::endl;
    // }
    return std::make_pair(-4,-0.0);
  }

  std::cout << "should not reach" << std::endl;
  return std::make_pair(-5,-0.0);
}

double LP_lower(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats) {

  int64_t l = mesh.size();
  int64_t N = dist.N;

  std::unordered_map<int64_t, double> good_turing_estimates;
  for (auto x:dist.freqcount) {
    good_turing_estimates[x.first] = ((double) x.first * x.second) / (N - x.first + 1.0);
  }

  try {
    GRBEnv env = GRBEnv(true);
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();

    GRBModel model = GRBModel(env);

    // variables
    std::vector<GRBVar> hx_vars(l); // hx_vars[i] is h_j * x_j
    for (int i=0; i<l; ++i) {
      hx_vars[i] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "hx_vars["+std::to_string(i)+"]");
    }
    GRBVar p_var = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "p_var");
    GRBVar c_var = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "c_var"); // c_var is c * x_idx

    // objective
    GRBLinExpr obj = 0.0;
    for (int i=0; i<idx-1; ++i) { // idx-1 is because of 1-indexing in paper vs. 0-indexing in program
      obj += hx_vars[i];
    }
    if (idx <= l) {
      obj += c_var;
    }
    model.setObjective(obj, GRB_MINIMIZE);

    // constraints

    // constraint (1)
    GRBLinExpr constr_1_lhs = 0.0;
    double scale = (G/mesh[idx-1]>=1.0) ? 1.0/G : mesh[idx-1];
    for (int i=0; i<idx-1; ++i) {
      constr_1_lhs += hx_vars[i] * scale / mesh[i];
    }
    if (idx <= l) {
      constr_1_lhs += c_var * scale / mesh[idx-1];
    }
    GRBLinExpr constr_1_rhs = G * scale;
    model.addConstr(constr_1_lhs, GRB_EQUAL, constr_1_rhs, "1) (sum{j<idx} h_j) + c == G");

    // constraint (2)
    for (int i=0; i<=iprime; ++i) {
      GRBLinExpr sum_hxvars_bpdf = 0.0;
      GRBLinExpr lb = 0.0;
      GRBLinExpr ub = 0.0;
      for (int j=0; j<l; ++j) {
        sum_hxvars_bpdf += hx_vars[j] * bpdf(i, N, mesh[j]);
      }
      if (i==0) {
        lb = (1.0 / pow(q, i+1)) * (good_turing_estimates[i+1] - eps2s[i] - ((double) (i+1))/((double) (N-i)) - p_var);
        ub = (1.0 + eps3s[i]) * (good_turing_estimates[i+1] + eps2s[i] - p_var * bpdf(i, N, q * mesh[l-1])) + bpdf(i, N, xhats[i]);
      }
      else {
        lb = (1.0 / pow(q, i+1)) * (good_turing_estimates[i+1] - eps2s[i] - ((double) (i+1))/((double) (N-i)) - p_var * bpdf(i, N, q * mesh[l-1]));
        ub = (1.0 + eps3s[i]) * (good_turing_estimates[i+1] + eps2s[i]) + bpdf(0, N, xhats[i]);
      }
      model.addConstr(lb, GRB_LESS_EQUAL, sum_hxvars_bpdf, "2) lb");
      model.addConstr(sum_hxvars_bpdf, GRB_LESS_EQUAL, ub, "2) ub");
    }
    
    // constraint (3)
    GRBLinExpr left = (1.0 - p_var) / q;
    GRBLinExpr right = 1.0 - p_var;
    GRBLinExpr sum_hxvars = 0.0;
    for (int i=0; i<l; ++i) {
      sum_hxvars += hx_vars[i];
    }
    model.addConstr(left, GRB_LESS_EQUAL, sum_hxvars, "3) 1-p/q <= sum h_j");
    model.addConstr(sum_hxvars, GRB_LESS_EQUAL, right, "3) sum h_j <= 1-p");
    
    // constraint (4)
    if (idx <= l) {
      model.addConstr(c_var <= hx_vars[idx-1], "4) c <= h_idx");
    }


    // optimize 
    model.optimize();

    // status/solution
    int status = model.get(GRB_IntAttr_Status);
		if (status == GRB_OPTIMAL) {
      return model.get(GRB_DoubleAttr_ObjVal);
		}
    else if (status == GRB_INFEASIBLE) {
      return -2;
    }
    else {
      return -1;
    }
  } catch(GRBException e) {
    if (dist.verbose) {
      std::cerr << "\n[Error: code = " << e.getErrorCode() << "; message: " << e.getMessage() << ".]" << std::endl;
    }
    return -3;
  } catch(...) {
    if (dist.verbose) {
      std::cerr << "\n[Error: Exception during optimization.]" << std::endl;
    }
    return -4;
  }

  return 0.0;
}

double LP_upper(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats) {

  int64_t l = mesh.size();
  int64_t N = dist.N;

  std::unordered_map<int64_t, double> good_turing_estimates;
  for (auto x:dist.freqcount) {
    good_turing_estimates[x.first] = ((double) x.first * x.second) / (N - x.first + 1.0);
  }

  try {
    GRBEnv env = GRBEnv(true);
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();

    GRBModel model = GRBModel(env);

    // variables
    std::vector<GRBVar> hx_vars(l); // hx_vars[i] is h_j * x_j
    for (int i=0; i<l; ++i) {
      hx_vars[i] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "hx_vars["+std::to_string(i)+"]");
    }
    GRBVar p_var = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "p_var");
    GRBVar c_var = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "c_var"); // c_var is c * x_idx

    // objective
    GRBLinExpr obj = 0.0;
    for (int i=0; i<idx-1; ++i) { // idx-1 is because of 1-indexing in paper vs. 0-indexing in program
      obj += hx_vars[i];
    }
    obj += c_var;
    model.setObjective(obj, GRB_MAXIMIZE);

    // constraints

    // constraint (1)
    GRBLinExpr constr_1_lhs = 0.0;
    double scale = (G/mesh[idx-1]>=1.0) ? 1.0/G : mesh[idx-1];
    for (int i=0; i<idx-1; ++i) {
      constr_1_lhs += hx_vars[i] * scale / mesh[i];
    }
    if (idx <= l) {
      constr_1_lhs += c_var * scale / mesh[idx-1];
    }
    else {
      constr_1_lhs += c_var * scale / mesh[l-1];
    }
    GRBLinExpr constr_1_rhs = G * scale;
    model.addConstr(constr_1_lhs, GRB_EQUAL, constr_1_rhs, "1) (sum{j<idx} h_j) + c == G");

    // constraint (2)
    for (int i=0; i<=iprime; ++i) {
      GRBLinExpr sum_hxvars_bpdf = 0.0;
      GRBLinExpr lb = 0.0;
      GRBLinExpr ub = 0.0;
      for (int j=0; j<l; ++j) {
        sum_hxvars_bpdf += hx_vars[j] * bpdf(i, N, mesh[j]);
      }
      if (i==0) {
        lb = (1.0 / (1.0 + eps3s[i])) * (good_turing_estimates[i+1] - eps2s[i] - ((double) (i+1))/((double) (N-i)) - p_var - bpdf(i, N, q * xhats[i]));
        ub = pow(q, i+1) * (good_turing_estimates[i+1] + eps2s[i] - p_var * bpdf(i, N, q * mesh[l-1]));
      }
      else {
        lb = (1.0 / (1.0 + eps3s[i])) * (good_turing_estimates[i+1] - eps2s[i] - ((double) (i+1))/((double) (N-i)) - p_var * bpdf(i, N, q * mesh[l-1]) - bpdf(i, N, q * xhats[i]));
        ub = pow(q, i+1) * (good_turing_estimates[i+1] + eps2s[i]);
      }
      model.addConstr(lb, GRB_LESS_EQUAL, sum_hxvars_bpdf);
      model.addConstr(sum_hxvars_bpdf, GRB_LESS_EQUAL, ub);
    }
    
    // constraint (3)
    GRBLinExpr left = 1.0 - p_var;
    GRBLinExpr right = (1.0 - p_var) * q;
    GRBLinExpr sum_hxvars = 0.0;
    for (int i=0; i<l; ++i) {
      sum_hxvars += hx_vars[i];
    }
    model.addConstr(left, GRB_LESS_EQUAL, sum_hxvars, "3) 1-p <= sum h_j");
    model.addConstr(sum_hxvars, GRB_LESS_EQUAL, right, "3) sum h_j <= q*(1-p)");
    
    // constraint (4)
    if (idx <= l) {
      model.addConstr(c_var <= hx_vars[idx-1], "4) c <= h_idx");
    }

    // optimize 
    model.optimize();

    // status/solution
    int status = model.get(GRB_IntAttr_Status);
		if (status == GRB_OPTIMAL) {
      return model.get(GRB_DoubleAttr_ObjVal);
		}
    else if (status == GRB_INFEASIBLE) {
      return -2;
    }
    else {
      return -1;
    }
  } catch(GRBException e) {
    if (dist.verbose) {
      std::cerr << "\n[Error: code = " << e.getErrorCode() << "; message: " << e.getMessage() << ".]" << std::endl;
    }
    return -3;
  } catch(...) {
    if (dist.verbose) {
      std::cerr << "\n[Error: Exception during optimization.]" << std::endl;
    }
    return -4;
  }

  return 1.0;
}

double LP_LB(dist_t& dist, int64_t G, double q, int64_t iprime, std::vector<double> errs, std::vector<double> xhats) {
  // Note: error rate will be 2 * sum(errs)
  if (!error_check_LP(dist, G, q, iprime, errs, xhats)) {
    return -1;
  }

  int64_t N = dist.N;
  std::vector<double> eps2s(iprime+1);
  std::vector<double> eps3s(iprime+1);
  for (int i=0; i<=iprime; ++i) {
    eps2s[i] = sqrt(-N * log(errs[i]) / 2.0) * ((((double) i) + 1.0) / ((double) (N - i)));
    double log_eps3 = (N - i) * log((1 - xhats[i]) / (1 - q*xhats[i])) - (i + 1) * log(q);
    eps3s[i] = exp(log_eps3) - 1;
  }

  int64_t l = ((int64_t) floor((log(10000.0) + log((double) N)) / log(q))) + 1;
  std::vector<double> mesh(l);
  mesh[l-1] = 1.0 / (10000.0 * N);
  for (int i=l-2; i>=0; --i) {
    mesh[i] = mesh[i+1] * q;
  }

  double res = 1.0;
  double lp_bound;
  bool feasible = false;
  for (int64_t idx=1; idx<=l+1; ++idx) {
    lp_bound = LP_lower(dist, G, mesh, q, iprime, idx, eps2s, eps3s, xhats);
    if (lp_bound > 0) {
      feasible = true;
      res = std::min(res, lp_bound);
    }
  }
  if (!feasible) {
    if (dist.verbose) {
      std::cerr << "\n[Model is infeasible! Sample might not be drawn iid from the underlying distribution.]" << std::endl;
    }
    return -2;
  }
  return std::max(res, 0.0);
}

double LP_UB(dist_t& dist, int64_t G, double q, int64_t iprime, std::vector<double> errs, std::vector<double> xhats) {
  // Note: error rate will be 2 * sum(errs)
  if (!error_check_LP(dist, G, q, iprime, errs, xhats)) {
    return -1;
  }

  int64_t N = dist.N;

  std::vector<double> eps2s(iprime+1);
  std::vector<double> eps3s(iprime+1);
  for (int i=0; i<=iprime; ++i) {
    eps2s[i] = sqrt(-N * log(errs[i]) / 2.0) * ((((double) i) + 1.0) / ((double) (N - i)));
    double log_eps3 = (N - i) * log((1 - xhats[i]) / (1 - q*xhats[i])) - (i + 1) * log(q);
    eps3s[i] = exp(log_eps3) - 1;
  }

  int64_t l = ((int64_t) floor((log(10000.0) + log((double) N)) / log(q))) + 1;
  std::vector<double> mesh(l);
  mesh[l-1] = 1.0 / (10000.0 * N);
  for (int i=l-2; i>=0; --i) {
    mesh[i] = mesh[i+1] * q;
  }

  double res = 0.0;
  double lp_bound;
  bool feasible = false;
  for (int64_t idx=1; idx<=l+1; ++idx) {
    lp_bound = LP_upper(dist, G, mesh, q, iprime, idx, eps2s, eps3s, xhats);
    if (lp_bound > 0) {
      feasible = true;
      res = std::max(res, lp_bound);
    }
  }
  if (!feasible) {
    if (dist.verbose) {
      std::cerr << "\n[Model is infeasible! Sample might not be drawn iid from the underlying distribution.]" << std::endl;
    }
    return -2;
  }
  return std::min(res, 1.0);
}

double LP_LB(dist_t& dist, int64_t G, double err) {
  return LP_LB_normal(dist, G, err);
}

double LP_UB(dist_t& dist, int64_t G, double err) {
  return LP_UB_normal(dist, G, err);
}

double LP_LB_fast(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }
  double q = 1.008;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_LB(dist, G, q, iprime, errs, xhats);
}

double LP_UB_fast(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }
  double q = 1.008;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_UB(dist, G, q, iprime, errs, xhats);
}

double LP_LB_normal(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }

  double q = 1.004;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_LB(dist, G, q, iprime, errs, xhats);
}

double LP_UB_normal(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }

  double q = 1.004;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_UB(dist, G, q, iprime, errs, xhats);
}

double LP_LB_slow(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }

  double q = 1.002;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_UB(dist, G, q, iprime, errs, xhats);
}

double LP_UB_slow(dist_t& dist, int64_t G, double err) {
  if (!error_check_basic(dist, G, err)) {
    return -1;
  }

  double q = 1.002;
  int64_t iprime = 6;
  std::vector<double> xhats = {3.0/dist.N, 4.0/dist.N, 5.0/dist.N, 6.0/dist.N, 7.0/dist.N, 8.0/dist.N, 9.0/dist.N};
  std::vector<double> errs = {1, 1.5, 1.75, 1.875, 1.9375, 1.96875, 1.984375};
  double sum = std::accumulate(errs.begin(), errs.end(), 0.0);
  for (auto& x:errs) {
    x *= (err / (2.0 * sum));
  }
  return LP_UB(dist, G, q, iprime, errs, xhats);
}

