#include <string>
#include <fstream>
#include <vector>
using namespace std;

vector<string> readFile(string path){
  ifstream file; 
  file.open(path);
  string line;
  vector<string> output;
  if (file.is_open()) {
    while (!file.eof()) {
      getline(file, line);
      cout << line;
      output.push_back(line);
    }
  }
  file.close();
  return output;
}
