#include "pwdio.hpp"

void print(dist_t& d) {
  cout << "distribution (" << d.filetype << "): " << d.filename << '\n';
  cout << "Distinct: " << d.distinct << "; Total: " << d.N << '\n';
  cout << "freqcount:\n";
  for (auto x:d.freqcount) {
    cout << "  (" << x.first << ", " << x.second << ")\n";
  }
}

void parse_freqcount(dist_t& dist, vector<pair<int64_t, int64_t>>& freqcount) {
  sort(freqcount.rbegin(), freqcount.rend()); // sort descending

  vector<int64_t> preftotal(freqcount.size(), 0);
  vector<int64_t> prefcount(freqcount.size(), 0);

  preftotal[0] = freqcount[0].first * freqcount[0].second;
  prefcount[0] = freqcount[0].second;

  for (int i=1; i<prefcount.size(); ++i) {
    preftotal[i] = preftotal[i-1] + freqcount[i].first * freqcount[i].second;
    prefcount[i] = prefcount[i-1] + freqcount[i].second;
  }

  dist.freqcount = freqcount;
  dist.preftotal = preftotal;
  dist.prefcount = prefcount;
  dist.N = preftotal.back();
  dist.distinct = prefcount.back();
}

void read_plain(dist_t& dist, string filename) {
  ifstream fin(filename);
  if (!fin.is_open()) {
    cerr << "Error: can't open file " << filename << endl;
    return;
  }

  unordered_map<string, int64_t> hist;
  unordered_map<int64_t, int64_t> cnt;

  dist.filename = filename;
  dist.filetype = "plain";

  string pwd;
  while (getline(fin, pwd)) {
    hist[pwd]++;
  }
  fin.close();

  for (auto& it : hist) {
    cnt[it.second]++;
  }
  vector<pair<int64_t, int64_t>> freqcount;
  for (auto& it : cnt) {
    freqcount.push_back({it.first, it.second});
  }
  parse_freqcount(dist, freqcount);
}

void read_pwdfreq(dist_t& dist, string filename) { // pwd freq seperated with \t
  ifstream fin(filename);
  if (!fin.is_open()) {
    cerr << "Error: can't open file " << filename << endl;
    return;
  }

  dist.filename = filename;
  dist.filetype = "pwdfreq";

  unordered_map<int64_t, int64_t> cnt;
  string pwd, line;
  int64_t freq;
  while (getline(fin, line)) {
    istringstream ss(line);
    if (getline(ss, pwd, '\t') && ss >> freq) {
      cnt[freq]++;
    }
    else {
      cerr << "invalid line: " << line << endl;
    }
  }
  fin.close();

  vector<pair<int64_t, int64_t>> freqcount;
  for (auto& it : cnt) {
    freqcount.push_back({it.first, it.second});
  }
  parse_freqcount(dist, freqcount);
}

void read_freqcount(dist_t& dist, string filename) {
  ifstream fin(filename);
  if (!fin.is_open()) {
    cerr << "Error: can't open file " << filename << endl;
    return;
  }

  dist.filename = filename;
  dist.filetype = "pwdfreq";

  int64_t freq, count;
  string line;
  vector<pair<int64_t, int64_t>> freqcount;

  while (getline(fin, line)) {
    istringstream ss(line);
    if (ss >> freq >> count && ss.peek() == char_traits<char>::eof()) {
      freqcount.push_back({freq, count});
    }
    else {
      cerr << "invalid line: " << line << endl;
    }
  }
  fin.close();

  parse_freqcount(dist, freqcount);
}

void write_freqcount(dist_t& dist, string filename) { // each line is (freq count)
  ofstream fout(filename);
  if (!fout.is_open()) {
    cerr << "Error: can't open file " << filename << endl;
  }

  for (auto x : dist.freqcount) {
    fout << x.first << ' ' << x.second << '\n';
  }
  fout.close();
}

dist_t create(string filename, string filetype) {
  dist_t res;
  if (filetype == "plain") {
    read_plain(res, filename);
  }
  else if (filetype == "pwdfreq") {
    read_pwdfreq(res, filename);
  }
  else if (filetype == "freqcount") {
    read_freqcount(res, filename);
  }
  else {
    cerr << "Error: " << filetype << " is not a valid filetype" << endl;
  }
  return res;
}


