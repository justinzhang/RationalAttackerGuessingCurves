#include "helpers.hpp"

vector<double> logs = {0};
vector<double> logpref = {0};

double fpow(double a, int64_t p) {
  double ans = 1.0;
  for (; p; a=a*a, p<<=1) {
    if (p&1) {
      ans *= a;
    }
  }
  return ans;
}

void populate_logs(int64_t N) {
  if (logs.size() > N) {
    return;
  }
  int64_t old_size = logs.size();
  logs.resize(N+1);
  logpref.resize(N+1);
  for (int64_t i=old_size; i<=N; ++i) {
    logs[i] = log2((double) i);
    logpref[i] = logpref[i-1] + logs[i];
  }
}

double logbpdf(int64_t i, int64_t N, double p) {
  populate_logs(N);
  return logpref[N] - logpref[i] - logpref[N-i] + log2(p)*i + log2(1-p)*(N-i);
}

double bpdf(int64_t i, int64_t N, double p) {
  return pow(2, logbpdf(i, N, p));
}

