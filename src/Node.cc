#include "Node.h"
#include <iostream>
Define_Module(Node);

Logs *Node::L01;
Logs *Node::L23;
Logs *Node::L45;
int Node::finishedNodesCount01;
int Node::finishedNodesCount23;
int Node::finishedNodesCount45;

void Node::initialize()
{
    // set the node id and initialize logs pointer
    if (strcmp("node0", getName()) == 0)
    {
        id = 0;
        if (L01 == NULL)
            L01 = new Logs("pair01.txt");
        finishedNodesCount01 = 0;
    }

    else if (strcmp("node1", getName()) == 0)
    {
        id = 1;
        if (L01 == NULL)
            L01 = new Logs("pair01.txt");
        finishedNodesCount01 = 0;
    }

    else if (strcmp("node2", getName()) == 0)
    {
        id = 2;
        if (L23 == NULL)
            L23 = new Logs("pair23.txt");
        finishedNodesCount23 = 0;
    }

    else if (strcmp("node3", getName()) == 0)
    {
        id = 3;
        if (L23 == NULL)
            L23 = new Logs("pair23.txt");
        finishedNodesCount23 = 0;
    }

    else if (strcmp("node4", getName()) == 0)
    {
        id = 4;
        if (L45 == NULL)
            L45 = new Logs("pair45.txt");
        finishedNodesCount45 = 0;
    }

    else if (strcmp("node5", getName()) == 0)
    {
        id = 5;
        if (L45 == NULL)
            L45 = new Logs("pair45.txt");
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
}

void Node::initializeMessages(cMessage *msg)
{
    // get the 0->node id, 1->text file name, 2->whether it's the starting node, 3->starting time
    vector<string>
        lineReceived = split(msg->getName(), ' ');

    // store file lines
    events = readFile(getBasePath() + "/inputs/" + lineReceived[1]);

    // check if I am the start node
    if (lineReceived.size() > 2)
    {
        startTime = stod(lineReceived[3]);
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
    if (indexToCheck >= events.size())
    {
        if (id == 0 or id == 1)
            finishedNodesCount01++;
        else if (id == 2 or id == 3)
            finishedNodesCount23++;
        else if (id == 4 or id == 5)
            finishedNodesCount45++;
        if (id == 0)
            return true;
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
    L01->addLog(id, 1, receivedSeqNum, mmsg->getM_Payload(), simTime().dbl(), !valid, 1, mmsg->getP_ack());

    cout << "receivingWindowStartIndex= " << receivingWindowStartIndex << endl;
    for (int i = 0; i < receivingWindow.size(); i++)
        cout
            << receivingWindow[i] << " ";
    cout << endl;

    cout << "receivedSeqNum= " << receivedSeqNum << endl;

    // check for duplicate messages
    if (receivedSeqNum < receivingWindowStartIndex || receivingWindow[receivedSeqNum - receivingWindowStartIndex])
    {
        // drop message
        L01->addLog(id, 2, receivedSeqNum, "", simTime().dbl(), 0, 0, 0);
        // send NACK on the first message in the receiving window
        messageToSendBack->setP_ack(1);
    }

    // if I received the expected frame
    else if (receivedSeqNum == receivingWindowStartIndex) // potential ack if the message is not modified
    {
        cout << "receivedSeqNum == receivingWindowStartIndex" << endl;
        if (valid) // message is correct send ack
        {
            cout << "valid" << endl;
            receivingWindow[0] = true;
            while (receivingWindow[0])
            {
                receivingWindow.erase(receivingWindow.begin()); // pop
                receivingWindow.push_back(false);               // push
                receivingWindowStartIndex++;
            }
            messageToSendBack->setP_ack(0); // ack
        }
        else // message is modified send nck
        {
            messageToSendBack->setP_ack(1); // nck
        }
    }

    // if I received a frame within the window
    else
    {
        cout << "else2" << endl;
        // check if received frame is valid
        if (valid)
        {
            cout << "valid in else" << endl;
            cout << "ind= " << receivedSeqNum - receivingWindowStartIndex << endl;
            receivingWindow[receivedSeqNum - receivingWindowStartIndex] = true;
        }
        // send NACK on the first frame in window (still not received)
        messageToSendBack->setP_ack(1);
    }
    messageToSendBack->setP_id(receivingWindowStartIndex);

    // TODO:delete those
    //  nck 1) message modified 2) wrong seq number

    //[true,false,false,false,false]
    //[0   ,1    ,2      ,3   ,4    ]

    //[true,true,true,true,false]

    //[true,true,false,true,false]

    //[,false,true,true,false,false]

    //        [false,false]
    //        start=1
    //[0   |,1    ,2 |     ,3   ,4    ]
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

    // received nck, resend the frame we received nack for
    if (receivedNck && receivedAckId >= receivingWindowStartIndex)
    {
        MyMessage_Base *messageToSendBack = new MyMessage_Base();
        formulateAndSendMessage(receivedAckId, messageToSendBack, false);
    }
    else
    { // recieved ack,possible to advance the window

        int cancelTimeoutCount = receivedAckId - sendingWindowStartIndex;
        sendingWindowStartIndex = receivedAckId; // shift the sending window
        // for (int i = 0; i < cancelTimeoutCount; i++)
        // {
        //     cancelEvent(timeoutMessages[i]);
        // }
        if (checkEndingCondition(sendingWindowStartIndex)) // end condition
            return;                                        // do nothing
        // schedule
        handleReadyToSend(msg, messageToSendBack);
    }
    //[0,1,2,3,4,5,|6,7,8,9,10|] 11
    // start=2
    // current=4
}

void Node::handleReadyToSend(cMessage *msg, MyMessage_Base *messageToSendBack)
{

    // if next frame to send is within window
    int relativeInd = nextFrameSeqNum - sendingWindowStartIndex;
    if (nextFrameSeqNum < events.size() && relativeInd < windowSize)
    {
        formulateAndSendMessage(nextFrameSeqNum, messageToSendBack);                                    // send next message
        nextFrameSeqNum++;                                                                              // move index to message after
        scheduleAt(simTime() + par("consecutiveDelay").doubleValue(), new cMessage("nextFrameToSend")); // schedule a self message of type READY_TO_SEND for next frame
    }
}

void Node::handleFrameArrival(cMessage *msg)
{

    MyMessage_Base *messageToSendBack = new MyMessage_Base();

    if (startTime == -1) // sender
    {
        handleReceivingMessage(msg, messageToSendBack);
        double delay = 0.2;
        sendDelayed(messageToSendBack, delay, "peerLink$o");
    }
    else // Receiver
    {
        handleReceivingAck(msg, messageToSendBack);
    }

    // node0
    //  [ data1,data2, data3....datan]

    // node1
    //[false,false,true,true,false]
    //[0   ,1    ,2      ,3   ,4    ]
}

void Node::handleTimeout(cMessage *msg)
{

    // check which frame in the window timedout
    for (int i = 0; i < windowSize; i++)
    {
        if (msg == timeoutMessages[i])
        {
            MyMessage_Base *messageToSendBack = new MyMessage_Base();
            formulateAndSendMessage(sendingWindowStartIndex + i, messageToSendBack);
            return;
        }
    }
}

void Node::handleMessage(cMessage *msg)
{
    messageType Type = getMessageType(msg);
    MyMessage_Base *messageToSendBack;

    switch (Type)
    {
    // check if the received message is from the coordinator
    case COORDINATOR_MESSAGE:
        initializeMessages(msg);
        break;
    case READY_TO_SEND:
        EV << "READY_TO_SEND" << endl;

        messageToSendBack = new MyMessage_Base();
        handleReadyToSend(msg, messageToSendBack);
        break;
    case FRAME_ARRIVAL:
        EV << "FRAME_ARRIVAL" << endl;

        handleFrameArrival(msg);
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

    // send only if not LOST
    if (!isLost)
    {
        // if message is not lost or modified, increment the number of correct messages
        if (!isModified)
            L01->incrementCorrectMessages(1);

        // if not duplicated or delayed
        if (!isDuplicated && !isDelayed)
        {
            // TODO: change ack number in phase 2
            L01->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1); // add a log
            send(messageToSend, "peerLink$o");                                                                // send to my peer
            L01->incrementTransNum(1);
        }

        // if message is duplicated but not delayed
        else if (isDuplicated && !isDelayed)
        {

            L01->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1);

            send(messageToSend, "peerLink$o"); // send first message now

            MyMessage_Base *messageToSendDup = messageToSend->dup();

            sendDelayed(messageToSendDup, 0.01, "peerLink$o"); // send duplicate with 0.01s delay
            L01->incrementTransNum(2);
        }

        // if message is both duplicated and delayed
        else if (isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            L01->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, 1, 1);
            sendDelayed(messageToSend, delay, "peerLink$o"); // send first message after delay
            MyMessage_Base *messageToSendDup = messageToSend->dup();
            sendDelayed(messageToSendDup, delay + 0.01, "peerLink$o"); // send duplicate with delay+0.01s
            L01->incrementTransNum(2);
        }
        // if message is not duplicated and delayed
        else if (!isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            L01->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, 1, 1);
            sendDelayed(messageToSend, delay, "peerLink$o"); // send first message after delay
            L01->incrementTransNum(1);
        }
    }

    // if message is lost, just log it without sending
    else
    {
        L01->addLog(id, 0, eventIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1);
        L01->incrementTransNum(1);
    }

    // set timeout in case the receiver send no response
    // scheduleAt(simTime().dbl() + par("timeoutSeconds").doubleValue(), timeoutMessages[eventIndex - sendingWindowStartIndex]);

    // check if a node finished all its messages
    // TODO: put this logic where we check that the all messages in the last window are acknowledged
}

Node::~Node()
{

    // TODO: delete everything :)
    delete L01;
    delete L23;
    delete L45;
    for (int i = 0; i < windowSize; i++)
        delete timeoutMessages[i];
    delete startMessage;
}

// void Node::handleMessage(cMessage *msg)
//{
//     messageType Type= getMessageType(msg);
//
//     // check if the received message is from the coordinator
//     if (coordinatorMessage)
//     {
//         initializeMessages(msg);
//     }
//
//     // if the message is from myself (either because I scheduled a time to start or I set a timeout)
//     else if (msg->isSelfMessage())
//     {
//         cout << "Scheduled self messsage or timeout" << endl;
//         sendMessage(msg); // implemented down below as a class method
//     }
//
//     // if the message is from the other pair
//     else
//     {
//         // I am the receiver in phase 1 and should only send ack or nack
//         if (startTime == -1)
//         {
//             receiveMessage(msg);
//         }
//
//         // I am the sender in phase 1
//         else
//         {
//             cout << "I am the sender, sending id= " << nextFrameSeqNum << endl;
//             sendMessage(msg); // implemented down below as a class method
//         }
//     }
// }

// void Node::sendMessage(cMessage *msg)
// {
//     if (!(msg == timeoutMessages[0]))
//     {
//         // if the msg received is not scheduled that means it received a response before the
//         // timeout --> in that case we cancel the scheduled timeout event and we set another one
//         // down below at the end of the function.
//         // we also cancel and delete the received message since we are not gonna use it to check
//         //  for ack or nck in phase 1
//         cancelEvent(timeoutMessages[0]);
//         // Phase 1: we don't use the received message to check anything (ack and nack don't affect sent messages)
//         cancelAndDelete(msg); // delete right away for now
//     }

//     // cout<<"node "<<id<<" is now sending message number "<<nextFrameSeqNum<<endl;
//     EV << "node " << id << " is now sending message number " << nextFrameSeqNum << endl;

//     // terminating condition for phase 1 (sender has no other message to send)
//     if (nextFrameSeqNum >= events.size())
//     {
//         // phase 1: if node 0 (the sender) finished its input file, stop the simulation
//         if (id == 0)
//             L01->setTransTime(simTime().dbl() - startTime); // TODO: change in phase 2
//         L01->addEOF(id);                                    // add a log that the node reached the end of its input file
//         return;
//     }

//     formulateAndSendMessage();
// }

// void Node::receiveMessage(cMessage *msg)
// {
//     // cout<<"I am the receiver"<<endl;
//     try
//     {
//         MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
//         // valid= 1->ack, 0->nck
//         int valid = validCRC(mmsg->getM_Payload(), mmsg->getCRC());

//         // received message log
//         L01->addLog(id, 1, mmsg->getId(), mmsg->getM_Payload(), simTime().dbl(), !valid, 1, mmsg->getP_ack());

//         mmsg->setP_ack(valid);
//         if (valid)
//         {
//             expectedFrameId++; // if ack, request next frame
//         }
//         mmsg->setP_id(expectedFrameId);

//         // check for duplicate frame
//         // if (mmsg->getId() < expectedFrameId) // TODO: use this in phase 2
//         // {
//         //     // drop message
//         //     L->addLog(id, 2, mmsg->getId(), "", simTime().dbl(), 0, 0, 0);
//         // }
//         if (mmsg->getId() == prevFrameId) // duplicate
//         {
//             // drop message
//             L01->addLog(id, 2, mmsg->getId(), "", simTime().dbl(), 0, 0, 0);
//         }
//         else
//         {
//             prevFrameId = mmsg->getId();
//             // sent message log TODO: change the sent message id and ack number in phase 2
//             L01->addLog(id, 0, -1, "", simTime().dbl(), !valid, valid, expectedFrameId);

//             // sending message
//             double delay = 0.2;
//             sendDelayed(mmsg, delay, "peerLink$o");
//             // send(mmsg, "peerLink$o");
//             // L->incrementTransNum(1);
//         }
//     }
//     catch (...)
//     {
//         cout << "Casting error" << endl;
//     }
// }
