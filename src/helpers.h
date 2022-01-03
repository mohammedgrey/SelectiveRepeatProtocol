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

enum messageType
{
  COORDINATOR_MESSAGE,
  READY_TO_SEND,
  FRAME_ARRIVAL,
  TIMEOUT
};

inline string getBasePath()
{
  map<string, string> basePath;
  // TODO: add the hostname of your pc and the corresponding base path to the src directory to the basePath map
  // Note: to know your host name open the cmd and type hostname
  basePath["DESKTOP-SP2J27C"] = "C:/Users/fatem/OneDrive/Desktop/omnetpp-5.6.2/samples/SelectiveRepeatProtocol/src";
  basePath["DESKTOP-LA84EMV"] = "C:/omnetpp-5.7/samples/SelectiveRepeatProtocol/src";
  basePath["Baz"] = "E:/Uni Stuff/Senior II Standing/Computer Networks I/Project/Project-2021-Student/SelectiveRepeatProtocol/src";
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

inline string modifyMessage(string message, double randModIndex)
{
  // Generate a random number (0 to string size) --> character to modify
  int charToModify = (int)(randModIndex * message.size());
  bitset<8> chbits(message[charToModify]);

  // Generate a random number (0 to 7) --> bit to modify
  int bitToModify = (int)(randModIndex * 8);
  chbits[bitToModify] = !chbits[bitToModify];

  // Convert back to string and alter old message
  char newChar = (char)chbits.to_ulong();
  message[charToModify] = newChar;

  return message;
}

inline void constructMessage(string line, int id, bool isModified, double randModIndex, MyMessage_Base *messageToSend)
{

  string messageContent = line.substr(5);         // get the message content without the error bits
  string payload = byteStuffing(messageContent);  // frame the message
  string remainderCRC = getRemainderCRC(payload); // get the remainder when dividing the message (after byte stuffing) by the generator function

  if (isModified)
    payload = modifyMessage(payload, randModIndex); // modify the message if it should be

  // header
  messageToSend->setId(id);                      // message id
  messageToSend->setStart_Time(simTime().dbl()); // sending time

  // payload
  messageToSend->setM_Payload(payload.c_str()); // message itself as a string

  // trailer
  messageToSend->setCRC(remainderCRC.c_str()); // put the CRC in the trailer
}

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
