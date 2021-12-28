#ifndef __BITMANUPLATIONS_NODE_H_
#define __BITMANUPLATIONS_NODE_H_

#include <omnetpp.h>
#include "helpers.h"
#include "Logs.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
private:
  int id;                   // id of the current node
  bool firstMessage;        // flag to check if the received message is from the coordinator
  vector<string> events;    // vector of lines from the node's input file
  int eventsIndex;          // to keep track which line I am currently sending
  double startTime;         // for starting nodes only
  int expectedFrameId;      // used to check for duplicates (should make sense in phase 2)
  int prevFrameId;          // used to check for duplicates (for phase 1 only)
  cMessage *timeoutMessage; // used to handle timeouts
  cMessage *startMessage;   // used to send the start message

protected:
  static Logs *L01; // pointer to logs class of pair01
  static Logs *L23; // pointer to logs class of pair23
  static Logs *L45; // pointer to logs class of pair45
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  void sendMessage(cMessage *msg);
  void receiveMessage(cMessage *msg);
  void initializeMessages(cMessage *msg);
  virtual ~Node();
};

#endif
