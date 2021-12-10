#include "Node.h"
#include <iostream>
Define_Module(Node);

void Node::initialize()
{
    // set the node id and initialize logs pointer
    if (strcmp("node0", getName()) == 0) {
        id = 0;
        L= new Logs("pair01.txt");
    }
    else if (strcmp("node1", getName()) == 0) {
        id = 1;
        L= new Logs("pair01.txt");
    }
    else if (strcmp("node2", getName()) == 0) {
        id = 2;
        L= new Logs("pair23.txt");
    }
    else if (strcmp("node3", getName()) == 0) {
        id = 3;
        L= new Logs("pair23.txt");
    }
    else if (strcmp("node4", getName()) == 0) {
        id = 4;
        L= new Logs("pair45.txt");
    }
    else if (strcmp("node5", getName()) == 0){
        id = 5;
        L= new Logs("pair45.txt");
    }

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
        //cout << "Events length: " << events.size() << endl;

        // check if I am the start node
        //cout<<"line received length "<<lineReceived.size()<<endl;
        if (lineReceived.size() > 2)
        {
            //cout<<"I have a start time"<<endl;
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
        //cout<<"Scheduled self messsage"<<endl;
        sendMessage(); // implemented down below as a class method

    }


    // if the message is from the other pair
    else
    {
        //I am the receiver in phase 1 and should only send ack or nack
        if (startTime == -1)
        {
            //cout<<"I am the receiver"<<endl;
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

        // I am the sender in phase 1
        else
        {
            //cout<<"I am the sender"<<endl;
            sendMessage(); // implemented down below as a class method
        }
    }

}

void Node::sendMessage()
{
    //cout<<"node "<<id<<" is now sending message number "<<eventsIndex<<endl;
    EV<<"node "<<id<<" is now sending message number "<<eventsIndex<<endl;

    // terminating condition for phase 1 (sender has no other message to send)
    if (eventsIndex >= events.size()) {
        L->addEOF(id);                 //add a log that the node reached the end of its input file
        return;
    }

    //getting errors to be applied to message
    bool isModified= events[eventsIndex].substr(0,1) == "1"? true: false;
    bool isDelay= events[eventsIndex].substr(1,2) == "1"? true: false;
    bool isLost= events[eventsIndex].substr(2,3) == "1"? true: false;
    bool isDuplicate= events[eventsIndex].substr(3,4) == "1"? true: false;

    //constructing next message to be sent
    double randModIndex= par("randNum").doubleValue();    //generate a random number 0-1 to be used for the modification
    MyMessage_Base *messageToSend = constructMessage(events[eventsIndex], eventsIndex, isModified, randModIndex);

    //TODO: make sure parameters are correct -> how to choose ack number?
    L->addLog(id, 0, eventsIndex, messageToSend->getM_Payload(), simTime().dbl(), isModified, 1, 1);    //add a log


    // TODO: use isDelay, isLost, isDuplicate to simulate imperfect conditions
    send(messageToSend, "peerLink$o"); // send to my peer

    eventsIndex++;                  //increment events index to the next message
}
