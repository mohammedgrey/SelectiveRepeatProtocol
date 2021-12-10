#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>
#include <fstream>
#include <vector>
#include <bitset>
#include <iostream>
#include <omnetpp.h>
#include <cstring>
#include <map>
#include "utils/CRC.h"
#include "MyMessage_m.h"
#include <stdlib.h>
#include <sstream>
#define NUMBER_OF_NODES 6

using namespace std;
using namespace omnetpp;

inline string getBasePath()
{
  map<string, string> basePath;
  // TODO: add the getenv("HOSTNAME") of your pc and the corresponding base path to the src directory
  basePath["DESKTOP-SP2J27C"] = "C:/Users/fatem/OneDrive/Desktop/omnetpp-5.6.2/samples/SelectiveRepeatProtocol/src";
  return basePath[getenv("HOSTNAME")];
}

inline vector<string> readFile(string path)
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
      if (line != "")
        output.push_back(line);
    }
  }
  file.close();
  return output;
}

inline string byteStuffing(string message)
{
  char flag = '$';
  char esc = '/';
  string stuffedMessage = "";

  // Loop over message and insert esc before any data flag or data esc
  for (int ch = 0; ch < message.size(); ch++)
  {
    if (message[ch] == flag || message[ch] == esc)
    {
      stuffedMessage += esc;
    }
    stuffedMessage += message[ch];
  }

  // Insert flag at the beginning and end
  stuffedMessage = flag + stuffedMessage + flag;

  // Return the stuffed message
  return stuffedMessage;
}

inline string modifyMessage(string message)
{
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

inline MyMessage_Base *constructMessage(string line, int id, bool &isDelay, bool &isLost, bool &isDuplicate)
{
  // modification, loss, Duplication, Delay
  string MLDDbits = line.substr(0, 4);
  isLost = MLDDbits[1] == '1' ? true : false;
  isDuplicate = MLDDbits[2] == '1' ? true : false;
  isDelay = MLDDbits[3] == '1' ? true : false;

  string messageContent = line.substr(5);                  //get the message content without the error bits
  string payload = byteStuffing(messageContent);           //frame the message
  long long int remainderCRC = getRemainderCRC(payload);   //get the remainder when dividing the message (after byte stuffing) by the generator function
  payload = modifyMessage(payload);

  MyMessage_Base *messageToSend = new MyMessage_Base();

  // header
  messageToSend->setId(id);                                //message id
  messageToSend->setStart_Time(simTime().dbl());           //sending time

  // payload
  messageToSend->setM_Payload(payload.c_str());            //message itself as a string

  // trailer
  messageToSend->setCRC((int)remainderCRC);               //put the CRC in the trailer

  return messageToSend;
}

// inline vector<string> split(string str, char seperator)
// {
//   vector<string> strings;
//   int i = 0;
//   int startIndex = 0, endIndex = 0;
//   while (i <= str.size())
//   {
//     if (str[i] == seperator || i == str.size())
//     {
//       endIndex = i;
//       string subStr = "";
//       subStr.append(str, startIndex, endIndex - startIndex);
//       strings.push_back(subStr);
//       startIndex = endIndex + 1;
//     }
//     i++;
//   }
//   return strings;
// }
inline std::vector<std::string> split(std::string const &str, const char delim)
{
  std::vector<std::string> out;
  // create a stream from the string
  std::stringstream s(str);

  std::string s2;
  while (std::getline(s, s2, delim))
  {
    out.push_back(s2); // store the string in s2
  }
  return out;
}

#endif
