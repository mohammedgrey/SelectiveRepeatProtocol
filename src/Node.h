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
  int id;                  // id of the current node
  bool coordinatorMessage; // flag to check if the received message is from the coordinator
  double startTime;        // for starting nodes only
  int expectedFrameId;     // used to check for duplicates (should make sense in phase 2)
  int prevFrameId;         // used to check for duplicates (for phase 1 only)
  int windowSize;          // taken from ini file

  // window parameters
  // sending buffer
  vector<string> events;       // vector of lines from the node's input file
  int eventsIndex;             // sequence number. to keep track which line I am currently sending
  int sendingWindowStartIndex; // index of events where window starts: 0 <= windowStart < length(events)-windowSize
  int sendingWindowRelIndex;   // index of next frame to send from window: 0 <= index < length(window)
  // expectedAck= windowStartIndex=expectedFrameId

  // receiving buffer
  vector<bool> receivingWindow;  // keep track of which frames are receiving in the window
  int receivingWindowStartIndex; // start index of receiving window
  // expectedFrame=receivingWindowStartIndex  //sequence number of expected frame (if not received, send NACK)

  // messages
  vector<cMessage *> timeoutMessages; // used to handle timeouts
  cMessage *startMessage;             // used to send the start message

protected:
  // logs
  static Logs *L01; // pointer to logs class of pair01
  static Logs *L23; // pointer to logs class of pair23
  static Logs *L45; // pointer to logs class of pair45

  // finishing check variables
  static int finishedNodesCount01; // if it equals 2, then the pair has finished
  static int finishedNodesCount23; // if it equals 2, then the pair has finished
  static int finishedNodesCount45; // if it equals 2, then the pair has finished

  // functions
  virtual void initialize();                 // initialized data members
  void initializeMessages(cMessage *msg);    // fills events vector with messages from input file
  messageType getMessageType(cMessage *msg); // returns the type of the given message

  void handleReadyToSend(cMessage *msg);  // called when a message is ready to be sent
  void handleFrameArrival(cMessage *msg); // called when a frame is received-> checks messages and acks/nacks
  void handleTimeout(cMessage *msg);      // called when a timeout message is received

  virtual void handleMessage(cMessage *msg);
  void sendMessage(cMessage *msg);
  void receiveMessage(cMessage *msg);
  void formulateAndSendMessage(); // frames the message to be sent and applies errors
  virtual ~Node();
};

#endif
