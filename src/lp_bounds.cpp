#include "lp_bounds.hpp"
#include "helpers.hpp"

#include <iostream>
#include <unordered_map>

double LP_lower(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats, std::string logfilename) {

  int64_t l = mesh.size();
  int64_t N = dist.N;

  std::unordered_map<int64_t, double> good_turing_estimates;
  for (auto x:dist.freqcount) {
    good_turing_estimates[x.first] = ((double) x.first * x.second) / (N - x.first + 1.0);
  }

  try {
    GRBEnv env = GRBEnv(true);
    env.set("LogFile", logfilename);
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
    else {
      return -1.0;
    }
		// else if (status == GRB_SOLUTION_LIMIT) {
  //     std::cerr << "The number of solutions found reached the value specified in the SolutionLimit parameter" << std::endl;
		// }
		// else if (status == GRB_TIME_LIMIT) {
  //     std::cerr << "Optimization terminated because the time expended exceeded the value specified in the TimeLimit parameter." << std::endl;
		// }
    // else if (status == GRB_INFEASIBLE) {
    //   std::cerr << "The model is infeasible" << std::endl;
    // }
		// else {
  //     std::cerr << "Optimization was stopped with status code " << status << std::endl;
		// }

  } catch(GRBException e) {
    std::cerr << "Error code = " << e.getErrorCode() << std::endl;
    std::cerr << e.getMessage() << std::endl;
  } catch(...) {
    std::cerr << "Exception during optimization" << std::endl;
  }

  return 0.0;
}

double LP_upper(dist_t& dist, int64_t G, std::vector<double>& mesh, double q, int64_t iprime, int64_t idx, std::vector<double>& eps2s, std::vector<double>& eps3s, std::vector<double>& xhats, std::string logfilename) {

  int64_t l = mesh.size();
  int64_t N = dist.N;

  std::unordered_map<int64_t, double> good_turing_estimates;
  for (auto x:dist.freqcount) {
    good_turing_estimates[x.first] = ((double) x.first * x.second) / (N - x.first + 1.0);
  }

  try {
    GRBEnv env = GRBEnv(true);
    env.set("LogFile", logfilename);
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
    else {
      return -1.0;
    }
		// else if (status == GRB_SOLUTION_LIMIT) {
  //     std::cerr << "The number of solutions found reached the value specified in the SolutionLimit parameter" << std::endl;
		// }
		// else if (status == GRB_TIME_LIMIT) {
  //     std::cerr << "Optimization terminated because the time expended exceeded the value specified in the TimeLimit parameter." << std::endl;
		// }
  //   else if (status == GRB_INFEASIBLE) {
  //     std::cerr << "The model is infeasible" << std::endl;
  //   }
		// else {
  //     std::cerr << "Optimization was stopped with status code " << status << std::endl;
		// }

  } catch(GRBException e) {
    std::cerr << "Error code = " << e.getErrorCode() << std::endl;
    std::cerr << e.getMessage() << std::endl;
  } catch(...) {
    std::cerr << "Exception during optimization" << std::endl;
  }

  return 0.0;
}

double LP_LB(dist_t& dist, int64_t G, double q, int64_t iprime, std::vector<double> errs, std::vector<double> xhats, std::string logfilename) {
  if (errs.size() != iprime + 1) {
    std::cerr << "Error: errs must be of size iprime+1." << std::endl;
    return 0.0;
  }
  if (xhats.size() != iprime + 1) {
    std::cerr << "Error: xhats must be of size iprime+1." << std::endl;
    return 0.0;
  }

  int64_t N = dist.N;
  std::vector<double> eps2s(iprime+1);
  std::vector<double> eps3s(iprime+1);
  for (int i=0; i<=iprime; ++i) {
    eps2s[i] = sqrtl(-N * log(errs[i]) / 2.0) * (((double) (i + 1.0)) / ((double) (N - i)));
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
    lp_bound = LP_lower(dist, G, mesh, q, iprime, idx, eps2s, eps3s, xhats, logfilename);
    if (lp_bound > 0) {
      feasible = true;
      res = std::min(res, lp_bound);
    }
  }
  if (!feasible) {
    std::cerr << "Infeasible distribution!!!" << std::endl;
    return -1.0;
  }
  return res;
}

double LP_UB(dist_t& dist, int64_t G, double q, int64_t iprime, std::vector<double> errs, std::vector<double> xhats, std::string logfilename) {
  if (errs.size() != iprime + 1) {
    std::cerr << "Error: errs must be of size iprime+1." << std::endl;
    return 0.0;
  }
  if (xhats.size() != iprime + 1) {
    std::cerr << "Error: xhats must be of size iprime+1." << std::endl;
    return 0.0;
  }

  int64_t N = dist.N;

  std::vector<double> eps2s(iprime+1);
  std::vector<double> eps3s(iprime+1);
  for (int i=0; i<=iprime; ++i) {
    eps2s[i] = sqrtl(-N * log(errs[i]) / 2.0) * (((double) (i + 1.0)) / ((double) (N - i)));
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
    lp_bound = LP_upper(dist, G, mesh, q, iprime, idx, eps2s, eps3s, xhats, logfilename);
    if (lp_bound > 0) {
      feasible = true;
      res = std::max(res, lp_bound);
    }
  }
  if (!feasible) {
    std::cerr << "Infeasible distribution!!!" << std::endl;
    return -1.0;
  }
  return res;
}
