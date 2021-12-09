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

string byteStuffing(string message) {
  char flag = '$';
  char esc = '/';
  string stuffedMessage = "";

  // Loop over message and insert esc before any data flag or data esc
  for (int ch = 0; ch < message.size(); ch++) {
    if (message[ch] == flag || message[ch] == esc) {
      stuffedMessage += esc;
    }
    stuffedMessage += message[ch];
  }
  
  // Insert flag at the beginning and end
  stuffedMessage = flag + stuffedMessage + flag;

  // Return the stuffed message
  return stuffedMessage;
}
