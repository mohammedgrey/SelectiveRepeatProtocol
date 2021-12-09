#include "Node.h"
#include <iostream>
Define_Module(Node);

void Node::initialize()
{
    //set the node id
    if (strcmp("node0", getName()) == 0) id= 0;
    else if (strcmp("node1", getName()) == 0) id= 1;
    else if (strcmp("node2", getName()) == 0) id= 2;
    else if (strcmp("node3", getName()) == 0) id= 3;
    else if (strcmp("node4", getName()) == 0) id= 4;
    else if (strcmp("node5", getName()) == 0) id= 5;

    //initialize firstMessage with true
    firstMessage= true;

    //initialize startTime with -1 and change it only if the node is a starting one
    startTime= -1;
}

void Node::handleMessage(cMessage *msg)
{
    //check if the received message is from the coordinator
    if (firstMessage) {

        vector <string> data;                   //stores the text file name, whether it's the starting node, starting time
//        char *token = strtok(msg->getName(), " ");   //ignore first token which is the node number
//        while(token != NULL) {
//            data.push_back(token);
//            token = strtok(NULL, " ");
//        }
//
//        //store file lines
//        events = readFile(data[0]);
//
//        //if this is a sending node, set the start time as well
//        if(data.size()>1) {
//            startTime= data[2];
//        }

        //change the flag to false
        firstMessage= false;

    }

    //if the message is from the other pair
    else {

    }
}
