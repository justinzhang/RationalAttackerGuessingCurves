#include "helpers.hpp"

double fpow(double a, int64_t p) {
  double ans = 1.0;
  for (; p; a=a*a, p<<=1) {
    if (p&1) {
      ans *= a;
    }
  }
  return ans;
}

double bpdf(int64_t i, int64_t N, double p) {
  double res = 1.0;
  double q = 1.0 - p;
  for (int j=0; j<i; ++j) {
    res *= ((N - j) * p * q / j);
  }
  return res * pow(q, N - 2*i);
}

