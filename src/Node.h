#ifndef __BITMANUPLATIONS_NODE_H_
#define __BITMANUPLATIONS_NODE_H_

#include <omnetpp.h>
#include "helpers.h"
#include "Logs.h"

using namespace omnetpp;

class Node : public cSimpleModule
{
private:
  int id;                  // id of the current node
  bool coordinatorMessage; // flag to check if the received message is from the coordinator
  double startTime;        // for starting nodes only
  int expectedFrameId;     // used to check for duplicates (should make sense in phase 2)
  int prevFrameId;         // used to check for duplicates (for phase 1 only)
  int windowSize;          // taken from ini file
  bool firstTime;          // variable to check if the pair that won't start should start
  bool finished;           // variable to check if a specific node finished
  int shouldSendNck;       // To check whether we should send nck in handleReadyToSend or not

  // window parameters
  // sending buffer
  vector<string> events;       // vector of lines from the node's input file
  int sendingWindowStartIndex; // index of events where window starts: 0 <= windowStart < length(events)-windowSize
  int nextFrameSeqNum;         // index of next frame to send from window: 0 <= index < length(events)
  // expectedAck= windowStartIndex=expectedFrameId

  // receiving buffer
  vector<bool> receivingWindow;  // keep track of which frames are receiving in the window
  int receivingWindowStartIndex; // start index of receiving window
  // NOTE: no need to create a vector to store received messages as we only log them. Instead, keep the starting index only
  //  expectedFrame=receivingWindowStartIndex  //sequence number of expected frame (if not received, send NACK)

  // messages
  vector<cMessage *> timeoutMessages; // used to handle timeouts
  cMessage *startMessage;             // used to send the start message

protected:
  // logs
  static Logs *L01; // pointer to logs class of pair01
  static Logs *L23; // pointer to logs class of pair23
  static Logs *L45; // pointer to logs class of pair45

  static Logs *logs[3];

  // finishing check variables
  static int finishedNodesCount01; // if it equals 2, then the pair has finished
  static int finishedNodesCount23; // if it equals 2, then the pair has finished
  static int finishedNodesCount45; // if it equals 2, then the pair has finished

  // functions
  virtual void initialize();                                                     // initialized data members
  void initializeMessages(cMessage *msg);                                        // fills events vector with messages from input file
  messageType getMessageType(cMessage *msg);                                     // returns the type of the given message
  void handleReceivingMessage(cMessage *msg, MyMessage_Base *messageToSendBack); // For handling receiving the frame
  void handleReceivingMessageHamming(cMessage *msg, MyMessage_Base *messageToSendBack); // For handling receiving the frame (in case of hamming)
  void handleReceivingAck(cMessage *msg, MyMessage_Base *messageToSendBack);     // For handling receiving ack or nck
  bool checkEndingCondition(int indexToCheck);
  bool bothNodesFinished();                                                                                    // checks if both nodes have finished
  void handleReadyToSend(cMessage *msg, MyMessage_Base *messageToSendBack = nullptr, bool applyErrors = true); // called when a message is ready to be sent
  void handleFrameArrival(cMessage *msg);                                                                      // called when a frame is received-> checks messages and acks/nacks
  void handleTimeout(cMessage *msg);                                                                           // called when a timeout message is received

  virtual void handleMessage(cMessage *msg);
  void sendMessage(cMessage *msg);
  void receiveMessage(cMessage *msg);
  void formulateAndSendMessage(int eventIndex, MyMessage_Base *messageToSend, bool applyErrors = true); // frames the message to be sent and applies errors
  virtual ~Node();
};

#endif
