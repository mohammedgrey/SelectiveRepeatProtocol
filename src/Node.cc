#include "Node.h"
#include <iostream>
Define_Module(Node);

Logs* Node::L;

void Node::initialize()
{
    // set the node id and initialize logs pointer
    if (strcmp("node0", getName()) == 0 )
    {
        id = 0;
        if (L == NULL) L = new Logs("pair01.txt");
    }
    else if (strcmp("node1", getName()) == 0)
    {
        id = 1;
        if (L == NULL) L = new Logs("pair01.txt");
    }
    else if (strcmp("node2", getName()) == 0)
    {
        id = 2;
        L = new Logs("pair23.txt");
    }
    else if (strcmp("node3", getName()) == 0)
    {
        id = 3;
        L = new Logs("pair23.txt");
    }
    else if (strcmp("node4", getName()) == 0)
    {
        id = 4;
        L = new Logs("pair45.txt");
    }
    else if (strcmp("node5", getName()) == 0)
    {
        id = 5;
        L = new Logs("pair45.txt");
    }

    // initialize firstMessage with true
    firstMessage = true;

    // initialize startTime with -1 and change it only if the node is a starting one
    startTime = -1;

    // initialize the events index to the first event
    eventsIndex = 0;

    // initialize expected fram id
    expectedFrameId = 0;

    // initialize prev frame id with no prev (-1)
    prevFrameId = -1;
}

void Node::handleMessage(cMessage *msg)
{
    // check if the received message is from the coordinator
    if (firstMessage)
    {
        initializeMessages(msg);
    }

    // if the message is from myself (either because I scheduled a time to start or I set a timeout)
    else if (msg->isSelfMessage())
    {
        cout << "Scheduled self messsage" << endl;
        sendMessage(msg); // implemented down below as a class method
    }

    // if the message is from the other pair
    else
    {
        // I am the receiver in phase 1 and should only send ack or nack
        if (startTime == -1)
        {
            receiveMessage(msg);
        }

        // I am the sender in phase 1
        else
        {
            cout << "I am the sender, sending id= " << eventsIndex << endl;
            sendMessage(msg); // implemented down below as a class method
        }
    }
}

void Node::sendMessage(cMessage *msg)
{
    //Phase 1: we don't use the received message to check anything (ack and nack don't affect sent messages)
    cancelAndDelete(msg);   //delete right away for now

    // cout<<"node "<<id<<" is now sending message number "<<eventsIndex<<endl;
    EV << "node " << id << " is now sending message number " << eventsIndex << endl;

    // terminating condition for phase 1 (sender has no other message to send)
    if (eventsIndex >= events.size())
    {
        //phase 1: if node 0 (the sender) finished its input file, stop the simulation
        if (id == 0)
            L->setTransTime(simTime().dbl()); // TODO: change in phase 2
        L->addEOF(id);                        // add a log that the node reached the end of its input file
        return;
    }

    // getting errors to be applied to message
    string MLDD = events[eventsIndex].substr(0, 4);
    bool isModified = MLDD[0] == '1' ? true : false;
    bool isLost = MLDD[1] == '1' ? true : false;
    bool isDuplicated = MLDD[2] == '1' ? true : false;
    bool isDelayed = MLDD[3] == '1' ? true : false;

    // MODIFICATION
    double randModIndex = par("randNum").doubleValue();                                                           // generate a random number 0-1 to be used for the modification
    MyMessage_Base *messageToSend = constructMessage(events[eventsIndex], eventsIndex, isModified, randModIndex); // constructing message

    // send only if not LOST
    if (!isLost)
    {
        //if message is not lost or modified, increment the number of correct messages
        if (!isModified) L->incrementCorrectMessages(1);

        // if not duplicated or delayed
        if (!isDuplicated && !isDelayed)
        {
            // TODO: change ack number in phase 2
            L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1); // add a log
            send(messageToSend, "peerLink$o");                                                               // send to my peer
            L->incrementTransNum(1);
        }

        // if message is duplicated but not delayed
        else if (isDuplicated && !isDelayed)
        {
            L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1);
            send(messageToSend, "peerLink$o");                                                                               // send first message now
            MyMessage_Base *messageToSendDup = constructMessage(events[eventsIndex], eventsIndex, isModified, randModIndex); // construct duplicate
            sendDelayed(messageToSendDup, 0.01, "peerLink$o");                                                               // send duplicate with 0.01s delay
            L->incrementTransNum(2);
        }

        // if message is both duplicated and delayed
        else if (isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, 1, 1);
            sendDelayed(messageToSend, delay, "peerLink$o");                                                                 // send first message after delay
            MyMessage_Base *messageToSendDup = constructMessage(events[eventsIndex], eventsIndex, isModified, randModIndex); // construct duplicate
            sendDelayed(messageToSendDup, delay + 0.01, "peerLink$o");                                                       // send duplicate with delay+0.01s
            L->incrementTransNum(2);
        }
        // if message is not duplicated and delayed
        else if (!isDuplicated && isDelayed)
        {
            double delay = par("delaySeconds").doubleValue();
            L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl() + delay, isModified, 1, 1);
            sendDelayed(messageToSend, delay, "peerLink$o"); // send first message after delay
            L->incrementTransNum(1);
        }
    }

    // if message is lost, just log it without sending
    else
    {
        cout << "--------------------------------------------------" << endl;
        cout << "I am lost " << endl;
        cout << "--------------------------------------------------" << endl;

        L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1);
        L->incrementTransNum(1);
    }

    // increment events index to the next message
    eventsIndex++;
}

void Node::receiveMessage(cMessage *msg)
{
    // cout<<"I am the receiver"<<endl;
    try
    {
        MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
        // valid= 1->ack, 0->nck
        int valid = validCRC(mmsg->getM_Payload(), mmsg->getCRC());

        // received message log
        L->addLog(id, 1, mmsg->getId(), mmsg->getM_Payload(), simTime().dbl(), !valid, 1, mmsg->getP_ack());

        mmsg->setP_ack(valid);
        if (valid)
        {
            expectedFrameId++; // if ack, request next frame
        }
        mmsg->setP_id(expectedFrameId);

        // check for duplicate frame
        // if (mmsg->getId() < expectedFrameId) // TODO: use this in phase 2
        // {
        //     // drop message
        //     L->addLog(id, 2, mmsg->getId(), "", simTime().dbl(), 0, 0, 0);
        // }
        if (mmsg->getId() == prevFrameId) // TODO: add condition if id=0
        {
            // drop message
            L->addLog(id, 2, mmsg->getId(), "", simTime().dbl(), 0, 0, 0);
        }
        else
        {
            prevFrameId = mmsg->getId();
        }

        // sent message log TODO: change the sent message id and ack number in phase 2
        L->addLog(id, 0, -1, "", simTime().dbl(), !valid, valid, expectedFrameId);

        // sending message
        double delay = 0.2;
        sendDelayed(mmsg, delay, "peerLink$o");
        // send(mmsg, "peerLink$o");
        //L->incrementTransNum(1);
    }
    catch (...)
    {
        cout << "Casting error" << endl;
    }
}

void Node::initializeMessages(cMessage *msg)
{
    // get the 0->node id, 1->text file name, 2->whether it's the starting node, 3->starting time
    vector<string> lineReceived = split(msg->getName(), ' ');

    // store file lines
    events = readFile(getBasePath() + "/inputs/" + lineReceived[1]);
    // cout << "Events length: " << events.size() << endl;

    // check if I am the start node
    // cout<<"line received length "<<lineReceived.size()<<endl;
    if (lineReceived.size() > 2)
    {
        // cout<<"I have a start time"<<endl;
        startTime = stod(lineReceived[3]);
        // schedule a time to start
        scheduleAt(startTime, new cMessage(""));
    }
    // change the flag to false
    firstMessage = false;
    cancelAndDelete(msg);
}

Node::~Node()
{
    delete L;
}
