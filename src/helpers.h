#include <string>
#include <fstream>
#include <vector>
#include <bitset>
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

string modifyMessage(string message) {
  // Generate a random number (0 to string size) --> character to modify
  int charToModify = 1;
  bitset<8> chbits(message[charToModify]);

  // Generate a random number (0 to 7) --> bit to modify
  int bitToModify = 1;
  chbits[bitToModify] = !chbits[bitToModify];

  // Convert back to string and alter old message
  char newChar = (char)chbits.to_ulong();
  message[charToModify] = newChar;

  return message;
}
