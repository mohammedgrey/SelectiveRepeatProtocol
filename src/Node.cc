#include "Node.h"
#include <iostream>
Define_Module(Node);

Logs *Node::L01;
Logs *Node::L23;
Logs *Node::L45;
int Node::finishedNodesCount01;
int Node::finishedNodesCount23;
int Node::finishedNodesCount45;

Logs *Node::logs[3];

void Node::initialize()
{
    // set the node id and initialize logs pointer
    if (strcmp("node0", getName()) == 0)
    {
        id = 0;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair01.txt");
        finishedNodesCount01 = 0;
    }

    else if (strcmp("node1", getName()) == 0)
    {
        id = 1;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair01.txt");
        finishedNodesCount01 = 0;
    }

    else if (strcmp("node2", getName()) == 0)
    {
        id = 2;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair23.txt");
        finishedNodesCount23 = 0;
    }

    else if (strcmp("node3", getName()) == 0)
    {
        id = 3;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair23.txt");
        finishedNodesCount23 = 0;
    }

    else if (strcmp("node4", getName()) == 0)
    {
        id = 4;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair45.txt");
        finishedNodesCount45 = 0;
    }

    else if (strcmp("node5", getName()) == 0)
    {
        id = 5;
        if (logs[id / 2] == NULL)
            logs[id / 2] = new Logs("pair45.txt");
        finishedNodesCount45 = 0;
    }

    // initializing general parameters
    coordinatorMessage = true;                 // initialize coordinatorMessage with true
    startTime = -1;                            // initialize startTime with -1 and change it only if the node is a starting one
    expectedFrameId = 0;                       // initialize expected fram id
    prevFrameId = -1;                          // initialize prev frame id with no prev (-1)
    windowSize = par("windowSize").intValue(); // taken from ini file

    // initializing sender buffer parameters
    sendingWindowStartIndex = 0;
    nextFrameSeqNum = 0;

    // initializing receiving buffer parameters
    for (int i = 0; i < windowSize; i++)
        receivingWindow.push_back(false); // taken from ini file
    receivingWindowStartIndex = 0;

    // initializing message parameters
    for (int i = 0; i < windowSize; i++)
    {
        timeoutMessages.push_back(new cMessage("timeoutMessage")); // initialize timeout message
    }
    startMessage = new cMessage("startMessage"); // initialize the start message

    firstTime = true;

    finished = false;

    shouldSendNck = 0;
}

void Node::initializeMessages(cMessage *msg)
{
    // get the 0->node id, 1->text file name, 2->whether it's the starting node, 3->starting time
    vector<string> lineReceived = split(msg->getName(), ' ');

    // store file lines
    events = readFile(getBasePath() + "/inputs/" + lineReceived[1]);

    // check if I am the start node
    if (lineReceived.size() > 2)
    {
        // TODO: add conditions for all pairs
        startTime = stod(lineReceived[3]);
        logs[id / 2]->setStartTime(startTime);
        // schedule a time to start
        scheduleAt(startTime, startMessage);
    }
    // change the flag to false
    coordinatorMessage = false;
    cancelAndDelete(msg);
}

messageType Node::getMessageType(cMessage *msg)
{
    if (coordinatorMessage)
        return COORDINATOR_MESSAGE;
    if (msg->isSelfMessage())
    {
        for (int i = 0; i < windowSize; i++)
        {
            if (msg == timeoutMessages[i])
            {
                return TIMEOUT;
            }
        }
        return READY_TO_SEND;
    }
    return FRAME_ARRIVAL;
}

bool Node::checkEndingCondition(int indexToCheck)
{
    if (indexToCheck >= events.size() && !finished)
    {
        if (id == 0 || id == 1)
            finishedNodesCount01++;

        else if (id == 2 || id == 3)
            finishedNodesCount23++;

        else if (id == 4 || id == 5)
            finishedNodesCount45++;

        logs[id / 2]->addEOF(id); // end of file of node id

        finished = true;
        return true;
    }
    return false;
}

bool Node::bothNodesFinished()
{
    if (id == 0 || id == 1)
    {
        bool node0and1finished = finishedNodesCount01 >= 2;
        if (node0and1finished)
            logs[id / 2]->setTransTime(simTime().dbl()); // this function subtracts the start time internally
        return node0and1finished;
    }
    else if (id == 2 || id == 3)
    {
        bool node2and3finished = finishedNodesCount23 >= 2;
        if (node2and3finished)
            logs[id / 2]->setTransTime(simTime().dbl()); // this function subtracts the start time internally
        return node2and3finished;
    }
    else if (id == 4 || id == 5)
    {
        bool node4and5finished = finishedNodesCount45 >= 2;
        if (node4and5finished)
            logs[id / 2]->setTransTime(simTime().dbl()); // this function subtracts the start time internally
        return node4and5finished;
    }
    return false;
}

void Node::handleReceivingMessage(cMessage *msg, MyMessage_Base *messageToSendBack)
{
    MyMessage_Base *mmsg;
    try
    {
        mmsg = check_and_cast<MyMessage_Base *>(msg);
        // valid= 1->ack, 0->nck
    }
    catch (...)
    {
        cout << "Casting error handleReceivingMessage" << endl;
    }
    int valid = validCRC(mmsg->getM_Payload(), mmsg->getCRC());
    int receivedSeqNum = mmsg->getId();

    // received message log
    logs[id / 2]->addLog(id, 1, receivedSeqNum, mmsg->getM_Payload(), simTime().dbl(), !valid, mmsg->getP_ack(), mmsg->getP_id());

    // check for duplicate messages
    if (receivedSeqNum < receivingWindowStartIndex || receivingWindow[receivedSeqNum - receivingWindowStartIndex])
    {
        // drop message
        logs[id / 2]->addLog(id, 2, receivedSeqNum, "", simTime().dbl(), 0, 0, 0);
        // send NACK on the first message in the receiving window
        shouldSendNck = 1;
    }

    // if I received the expected frame
    else if (receivedSeqNum == receivingWindowStartIndex) // potential ack if the message is not modified
    {
        // cout << "receivedSeqNum == receivingWindowStartIndex" << endl;
        if (valid) // message is correct send ack
        {
            //[false,true,false,false,false]
            receivingWindow[0] = true;
            while (receivingWindow[0])
            {
                receivingWindow.erase(receivingWindow.begin()); // pop
                receivingWindow.push_back(false);               // push
                receivingWindowStartIndex++;
            }
            shouldSendNck = 0;
        }
        else // message is modified send nck
        {
            shouldSendNck = 1;
        }
    }

    // if I received a frame within the window
    else
    {
        // check if received frame is valid
        if (valid)
        {
            receivingWindow[receivedSeqNum - receivingWindowStartIndex] = true;
        }
        // send NACK on the first frame in window (still not received)
        shouldSendNck = 1;
    }
}

void Node::handleReceivingMessageHamming(cMessage *msg, MyMessage_Base *messageToSendBack)
{
    MyMessage_Base *mmsg;
    try
    {
        mmsg = check_and_cast<MyMessage_Base *>(msg);
        // valid= 1->ack, 0->nck
    }
    catch (...)
    {
        cout << "Casting error handleReceivingMessage" << endl;
    }
    string binaryPayload = getBinaryStringFromPayLoad(mmsg->getM_Payload());
    int r = 0;
    int n = getNewLength(binaryPayload, r);
    string msgWithParity = getMsgWithParity(binaryPayload, n, r);
    int valid = validCRC(mmsg->getM_Payload(), mmsg->getCRC());
    int receivedSeqNum = mmsg->getId();

    // received message log
    logs[id / 2]->addLog(id, 1, receivedSeqNum, mmsg->getM_Payload(), simTime().dbl(), !valid, mmsg->getP_ack(), mmsg->getP_id());
    if(!valid)
    {
        int errorPosition=0;
        string correctedString = doHamming(mmsg->getM_Payload(), errorPosition);
        //TODO: call hamming logs
        logs[id / 2]->addHammingLog(errorPosition, correctedString);
    }

    // check for duplicate messages
    if (receivedSeqNum < receivingWindowStartIndex || receivingWindow[receivedSeqNum - receivingWindowStartIndex])
    {
        // drop message
        logs[id / 2]->addLog(id, 2, receivedSeqNum, "", simTime().dbl(), 0, 0, 0);
        // send NACK on the first message in the receiving window
        shouldSendNck = 1;
    }

    // if I received the expected frame
    else if (receivedSeqNum == receivingWindowStartIndex) // potential ack if the message is not modified
    {
            receivingWindow[0] = true;
            while (receivingWindow[0])
            {
                receivingWindow.erase(receivingWindow.begin()); // pop
                receivingWindow.push_back(false);               // push
                receivingWindowStartIndex++;
            }
            shouldSendNck = 0;
    }

    // if I received a frame within the window
    else
    {
            receivingWindow[receivedSeqNum - receivingWindowStartIndex] = true;
            shouldSendNck = 1;
    }
}

void Node::handleReceivingAck(cMessage *msg, MyMessage_Base *messageToSendBack)
{
    MyMessage_Base *mmsg;
    try
    {
        mmsg = check_and_cast<MyMessage_Base *>(msg);
    }
    catch (...)
    {
        cout << "Casting error handleReceivingAck" << endl;
    }
    int receivedNck = mmsg->getP_ack();
    int receivedAckId = mmsg->getP_id();

    int cancelTimeoutCount = receivedAckId - sendingWindowStartIndex;
    sendingWindowStartIndex = receivedAckId > sendingWindowStartIndex ? receivedAckId : sendingWindowStartIndex;

    // shift the sending window
    for (int i = 0; i < cancelTimeoutCount; i++)
    {
        cout << "Canceling timeout with index = " << i << endl;
        cMessage *firstTimeoutEvent = timeoutMessages[0];
        cancelEvent(firstTimeoutEvent);

        timeoutMessages.erase(timeoutMessages.begin()); // pop the first message
        timeoutMessages.push_back(firstTimeoutEvent);   // push it at the end
    }

    // received nck, resend the frame we received nack for
    if (receivedNck && receivedAckId >= sendingWindowStartIndex)
    {
        if (receivedAckId < events.size())
        {
            MyMessage_Base *messageToSendBack = new MyMessage_Base();
            if (receivedAckId - sendingWindowStartIndex < windowSize)
                formulateAndSendMessage(receivedAckId, messageToSendBack, false);
        }
        else
        {
            checkEndingCondition(receivedAckId);
            handleReadyToSend(msg, messageToSendBack, false);
        }
    }
    else
    { // recieved ack,possible to advance the window

        checkEndingCondition(sendingWindowStartIndex);

        // schedule
        // TODO: think about this part, should you schedule or not
        if ((startTime == -1 && firstTime))
        {
            handleReadyToSend(msg, messageToSendBack);
            firstTime = false;
        } else if (finished) {
            handleReadyToSend(msg, messageToSendBack, false);
        }

    }
}

void Node::handleReadyToSend(cMessage *msg, MyMessage_Base *messageToSendBack, bool applyErrors)
{
    if (checkEndingCondition(sendingWindowStartIndex))
        return;
    if (bothNodesFinished())
        return; // check if both nodes finished

    // if next frame to send is within window
    int relativeInd = nextFrameSeqNum - sendingWindowStartIndex;
    if (relativeInd < windowSize)
    {
        if (messageToSendBack == nullptr)
            messageToSendBack = new MyMessage_Base();
        formulateAndSendMessage(nextFrameSeqNum, messageToSendBack, applyErrors);
        if (nextFrameSeqNum < events.size() - 1)
        {
            nextFrameSeqNum++;                                                                              // move index to message after
            scheduleAt(simTime() + par("consecutiveDelay").doubleValue(), new cMessage("nextFrameToSend")); // schedule a self message of type READY_TO_SEND for next frame
        }
    }
}

void Node::handleFrameArrival(cMessage *msg)
{
    MyMessage_Base *messageToSendBack = new MyMessage_Base();
    // sets the ack and nck for the message to send back along with ack ID
    if(par("useHammingCorrection").intValue() == 0)
    {
        handleReceivingMessage(msg, messageToSendBack);
    }
    else
    {
        handleReceivingMessageHamming(msg, messageToSendBack);
    }
    // sets the payload, crc, message ID of the message to send back and sends the message
    handleReceivingAck(msg, messageToSendBack);
}

void Node::handleTimeout(cMessage *msg)
{
    // check which frame in the window timedout
    for (int i = 0; i < windowSize; i++)
    {
        if (msg == timeoutMessages[i])
        {
            MyMessage_Base *messageToSendBack = new MyMessage_Base();
            formulateAndSendMessage(sendingWindowStartIndex + i, messageToSendBack, false);
            return;
        }
    }
}

void Node::handleMessage(cMessage *msg)
{
    // Exiting condition
    if (bothNodesFinished())
    {
        return;
    }

    messageType Type = getMessageType(msg);
    switch (Type)
    {
    // check if the received message is from the coordinator
    case COORDINATOR_MESSAGE:
        initializeMessages(msg);
        break;
    case READY_TO_SEND:
        EV << "READY_TO_SEND" << endl;
        handleReadyToSend(msg);
        cancelAndDelete(msg);
        break;
    case FRAME_ARRIVAL:
        EV << "FRAME_ARRIVAL" << endl;
        handleFrameArrival(msg);
        cancelAndDelete(msg);
        break;
    case TIMEOUT:
        EV << "TIMEOUT" << endl;
        handleTimeout(msg);
        break;
    }
}

// takes next message to be sent from events vector, frames it, applies errors, sends frame, triggers timeout, and increments index to next message
void Node::formulateAndSendMessage(int eventIndex, MyMessage_Base *messageToSend, bool applyErrors)
{
    // accessing the delay between consecutive messages to be used in the conditions below
    double consDelay = par("consecutiveDelay").doubleValue();
    bool isModified = false, isLost = false, isDuplicated = false, isDelayed = false;

    // getting errors to be applied to message
    if (applyErrors)
    {
        string MLDD = events[eventIndex].substr(0, 4);
        isModified = MLDD[0] == '1' ? true : false;
        isLost = MLDD[1] == '1' ? true : false;
        isDuplicated = MLDD[2] == '1' ? true : false;
        isDelayed = MLDD[3] == '1' ? true : false;
    }

    // MODIFICATION
    double randModIndex = par("randNum").doubleValue();                                        // generate a random number 0-1 to be used for the modification
    constructMessage(events[eventIndex], eventIndex, isModified, randModIndex, messageToSend); // constructing message
    messageToSend->setP_id(receivingWindowStartIndex);                                         // make sure we send the ack id correctly
    messageToSend->setP_ack(shouldSendNck);

    // send only if not LOST
    if (!isLost)
    {
        // if message is not lost or modified, increment the number of correct messages
        if (!isModified)
            logs[id / 2]->incrementCorrectMessages(1);

        // if not duplicated or delayed
        if (!isDuplicated && !isDelayed)
        {
            // TODO: change ack number in phase 2
            logs[id / 2]->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, messageToSend->getP_ack(), messageToSend->getP_id()); // add a log
            sendDelayed(messageToSend, consDelay, "peerLink$o");                                                                                                      // send to my peer
            logs[id / 2]->incrementTransNum(1);
        }
        // if message is duplicated but not delayed
        else if (isDuplicated && !isDelayed)
        {
            logs[id / 2]->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, messageToSend->getP_ack(), messageToSend->getP_id());
            sendDelayed(messageToSend, consDelay, "peerLink$o"); // send first message now
            MyMessage_Base *messageToSendDup = messageToSend->dup();
            sendDelayed(messageToSendDup, consDelay + par("duplicationDelay").doubleValue(), "peerLink$o"); // send duplicate with 0.01s delay
            logs[id / 2]->incrementTransNum(2);
        }
        // if message is both duplicated and delayed
        else if (isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            logs[id / 2]->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, messageToSend->getP_ack(), messageToSend->getP_id());
            sendDelayed(messageToSend, consDelay + delay, "peerLink$o"); // send first message after delay
            MyMessage_Base *messageToSendDup = messageToSend->dup();
            sendDelayed(messageToSendDup, consDelay + delay + par("duplicationDelay").doubleValue(), "peerLink$o"); // send duplicate with delay+0.01s
            logs[id / 2]->incrementTransNum(2);
        }
        // if message is not duplicated and delayed
        else if (!isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            logs[id / 2]->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, messageToSend->getP_ack(), messageToSend->getP_id());
            sendDelayed(messageToSend, consDelay + delay, "peerLink$o"); // send first message after delay
            logs[id / 2]->incrementTransNum(1);
        }
    }
    // if message is lost, just log it without sending
    else
    {
        logs[id / 2]->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, messageToSend->getP_ack(), messageToSend->getP_id());
        logs[id / 2]->incrementTransNum(1);
    }

    // set timeout in case the receiver send no response

    if (eventIndex >= sendingWindowStartIndex && eventIndex < events.size())
    {
        if (!timeoutMessages[eventIndex - sendingWindowStartIndex]->isScheduled())
        {
            cout << "Set time out for node = " << id << " and messageID= " << eventIndex << " and the current start of window is at " << sendingWindowStartIndex << endl;
            cancelEvent(timeoutMessages[eventIndex - sendingWindowStartIndex]);
            scheduleAt(simTime().dbl() + par("timeoutSeconds").doubleValue(), timeoutMessages[eventIndex - sendingWindowStartIndex]);
        }
    }
    // check if a node finished all its messages
    // TODO: put this logic where we check that the all messages in the last window are acknowledged
}

Node::~Node()
{
    // TODO: delete everything :)
    delete logs[id / 2];
    for (int i = 0; i < windowSize; i++)
    {
        cancelEvent(timeoutMessages[i]);
        delete timeoutMessages[i];
    }
    delete startMessage;
}