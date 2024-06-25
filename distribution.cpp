#include "distribution.hpp"

void print(dist_t& d) {
  cout << "-----------------\n";
  cout << "Dataset\n";
  cout << "Filename: " << d.filename << "; Filetype:" << d.filetype << '\n';
  cout << "Distinct: " << d.distinct << "; Total: " << d.N << '\n';
  cout << "freqcount:\n";
  for (auto x:d.freqcount) {
    cout << "  (" << x.first << ", " << x.second << ")\n";
  }
  cout << "-----------------\n";
}

int64_t most_frequent(dist_t& dist, int64_t G) { // cumulative frequency of top G most frequent passwords
  auto it = lower_bound(dist.prefcount.begin(), dist.prefcount.end(), G);
  if (it == dist.prefcount.begin()) {
    return G * dist.freqcount[0].first;
  }
  else if (it == dist.prefcount.end()) {
    return dist.N;
  }
  else {
    int64_t id = it - dist.prefcount.begin();
    return dist.preftotal[id] - (dist.prefcount[id] - G) * dist.freqcount[id].first;
  }
}

