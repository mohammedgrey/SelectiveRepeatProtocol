#include "Node.h"
#include <iostream>
Define_Module(Node);

void Node::initialize()
{
    // set the node id
    if (strcmp("node0", getName()) == 0)
        id = 0;
    else if (strcmp("node1", getName()) == 0)
        id = 1;
    else if (strcmp("node2", getName()) == 0)
        id = 2;
    else if (strcmp("node3", getName()) == 0)
        id = 3;
    else if (strcmp("node4", getName()) == 0)
        id = 4;
    else if (strcmp("node5", getName()) == 0)
        id = 5;

    // initialize firstMessage with true
    firstMessage = true;

    // initialize startTime with -1 and change it only if the node is a starting one
    startTime = -1;

    // initialize the events index to the first event
    eventsIndex = 0;

    // initialize piggybackingId with 0
    piggybackingId = 0;
}

void Node::handleMessage(cMessage *msg)
{
    // check if the received message is from the coordinator
    if (firstMessage)
    {
        // get the 0->node id, 1->text file name, 2->whether it's the starting node, 3->starting time
        vector<string> lineReceived = split(msg->getName(), ' ');
        // store file lines
        events = readFile(getBasePath() + "/inputs/" + lineReceived[1]);
        cout << "Events length: " << events.size() << endl;

        // check if I am the start node
        cout<<"line received length "<<lineReceived.size()<<endl;
        if (lineReceived.size() > 2)
        {
            cout<<"I have a start time"<<endl;
            startTime = stod(lineReceived[3]);
            // schedule a time to start
            scheduleAt(startTime, new cMessage(""));
        }
        // change the flag to false
        firstMessage = false;
        cancelAndDelete(msg);
    }
    // if the message is from myself (either because I scheduled a time to start or I set a timeout)
    else if (msg->isSelfMessage())
    {
        cout<<"Schduled self messsage"<<endl;
        sendMessage(); // implemented down below as a class method

    }
    // if the message is from the other pair
    else
    {
        if (startTime == -1) // I am the receiver in phase 1 and should only send ack or nck
        {
            cout<<"I am the receiver"<<endl;
            try
            {
                MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
                // 0->ack, 1->nck
                int ack =
                    validCRC(mmsg->getM_Payload(), (long long int)mmsg->getCRC()) ? 0 : 1;
                mmsg->setP_ack(ack);
                mmsg->setP_id(piggybackingId++);
                send(mmsg, "peerLink$o");
            }
            catch (...)
            {
                cout << "Casting error" << endl;
            }
        }
        else // I am the sender in phase 1
        {
            cout<<"I am the sender"<<endl;
            sendMessage(); // implemented down below as a class method
        }
    }
}

void Node::sendMessage()
{
    cout<<"node "<<id<<" is now sending message number "<<eventsIndex<<endl;
    EV<<"node "<<id<<" is now sending message number "<<eventsIndex<<endl;
    if (eventsIndex >= events.size())
        // terminating condition for phase 1 (sender has no other message to send)
        return;
    bool isDelay, isLost, isDuplicate;
    MyMessage_Base *messageToSend = constructMessage(events[eventsIndex], eventsIndex, isDelay, isLost, isDuplicate);
    eventsIndex++;
    // TODO: use isDelay, isLost, isDuplicate to simulate imperfect conditions
    send(messageToSend, "peerLink$o"); // send to my peer
}
