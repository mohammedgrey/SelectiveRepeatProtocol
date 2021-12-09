#include <string>
#include <fstream>
#include <vector>
#include <bitset>

using namespace std;
using namespace omnetpp;

vector<string> readFile(string path)
{
  ifstream file;
  file.open(path);
  string line;
  vector<string> output;
  if (file.is_open())
  {
    while (!file.eof())
    {
      getline(file, line);
      cout << line;
      output.push_back(line);
    }
  }
  file.close();
  return output;
}

int constructMessage(string line, bool &isDelay, bool &isLost, bool &isDuplicate)
{
  // modification, loss, Duplication, Delay
  string MLDDbits = line.substr(0, 4);
  isLost = MLDDbits[1] == '1' ? true : false;
  isDuplicate = MLDDbits[2] == '1' ? true : false;
  isDelay = MLDDbits[3] == '1' ? true : false;

  string messageContent = line.substr(5);
  string payload = messageContent; // TODO call byte stuffing function here
  double simTime = simTime();
}
