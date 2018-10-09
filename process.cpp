#ifdef   DEBUG
#define  VERBOSE
#else
#undef   VERBOSE
#endif

#define  DEBUG

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstring>

typedef long long ll;

using namespace std;

// Split string with specific separator.
vector<string> splitstr(string &line, char separator) {
  vector<string> parts;
  if(line.size() == 0) {
    return parts;
  }
  string part("");
  line.push_back(separator);
  for(int i = 0; i < line.size(); ++i) {
    if(line[i] == separator) {
      parts.push_back(string(part));
      part.clear();
    }
    else {
      part.push_back(line[i]);
    }
  }
  return parts;
}

// Analyse the specific format input string,
// get loop and correspond time cost.
void analyse(string &line, int &loop, ll &ts) {
  loop = 0;
  ts = 0;
  if(line.size() == 0) {
    return;
  }

  vector<string> parts = splitstr(line, ' ');
  for(int i = 0; i < parts[3].size() - 1; ++i) {
    loop = loop * 10 + int(parts[3][i] - '0');
  }

  for(int i = 0; i < parts[6].size(); ++i) {
    ts = ts * 10 + ll(parts[6][i] - '0');
  }

#ifdef VERBOSE
  cout << loop << " " << ts << endl;
#endif
}

// Reverse and build a new string.
string reverseString(string &source) {
  string ret("");
  if(source.size() == 0) {
    return ret;
  }
  for(int i = source.size() - 1; i >= 0; --i) {
    ret.push_back(source[i]);
  }
  return ret;
}

// Get file name from file path, it is the last segment.
string getFileNameFromPath(string &fullPath) {
  string fname("");
  if(fullPath.size() == 0) {
    return fname;
  }

  for(int i = fullPath.size() - 1; i >= 0 && fullPath[i] != '/'; --i) {
    fname.push_back(fullPath[i]);
  }
  return reverseString(fname);
}

// Process the test result data and get the analyse result.
unordered_map<int, vector<ll> > process(string &filePath) {
  ifstream file(filePath);
  unordered_map<int, vector<ll> > ret;
  string line;
  while(getline(file, line)) {
    int loop = 0;
    ll ts = 0;

    if(line[0] == 'S') {
      continue;
    }

    analyse(line, loop, ts);
    if(loop == 0 || ts == 0) {
      continue;
    }
    if(ret.count(loop) == 0) {
      ret[loop] = vector<ll>();
    }
    ret[loop].push_back(ts);
  }

  for(unordered_map<int, vector<ll> >::iterator it = ret.begin(); it != ret.end(); ++it) {
    sort(it->second.begin(), it->second.end());
  }

  return ret;
}

int main() {
    string fpath1("/Users/ruiguo.yang/Documents/Lab/mysql/performance2.txt");
    string fpath[] = {
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch1000-10.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch1000-100.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch1000-500.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-10.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-100.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-1000.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-2000.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-500.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-5000.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/nobatch1000.txt",
      "/Users/ruiguo.yang/Documents/Lab/mysql/nobatch10000.txt"
    };
    int length = sizeof(fpath) / sizeof(fpath[0]);
    vector<string> fileSet(fpath, fpath + length);
    unordered_map<string, unordered_map<int, vector<ll> > > processResult;

    for(int i = 0; i < fileSet.size(); ++i) {
      unordered_map<int, vector<ll> > itemProcessResult = process(fileSet[i]);
      string fileName = getFileNameFromPath(fileSet[i]);
      processResult[fileName] = itemProcessResult;
    }

    for(unordered_map<string, unordered_map<int, vector<ll> > >::iterator it = processResult.begin(); it != processResult.end(); ++it) {
      cout << it->first << ":" << endl;
      for(int i = 1; i <= 10; ++i) {
        if(it->second.count(i) == 0) {
          cout << "This file does't contain loop " << i << endl;
          continue;
        }
#ifdef VERBOSE
        cout << "\t" << "Loop " << i << ":" << endl;
#else
        cout << "\t\t" << "Loop " << i << "\t";
#endif

#ifdef VERBOSE
        // Output the orginal results.
        cout << "\t----------------start original----------------" << endl;
        cout << "\t  ";
        for(int j = 0; j < it->second[i].size(); ++j) {
          double second = it->second[i][j] / (10e9 * 1.0);
          cout << "[" << j << "]=" << second << ",";
        }
        cout << endl;

        // Output the delta of original results.
        cout << "\t----------------start delta----------------" << endl;
        cout << "\t ";
#endif
        ll sumDelta = 0;
        double count = 0;
        for(int j = 0; j < it->second[i].size() - 1; ++j) {
          ll delta = it->second[i][j + 1] - it->second[i][j];
#ifdef VERBOSE
          double second = delta / (10e9 * 1.0);
#endif
          sumDelta += delta;
          count += 1;
#ifdef VERBOSE
          cout << "[" << j << "]=" << delta << ",";
#endif
        }
#ifdef VERBOSE
        cout << endl;
#endif
        // Output the average of delta;
        cout << "\tThe average delta is ";
        double avgDelta = sumDelta / (count * 10e9);
        cout << avgDelta << endl;
      }
    }

    return 0;
}
