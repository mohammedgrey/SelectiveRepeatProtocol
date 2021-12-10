#include "Coordinator.h"

Define_Module(Coordinator);


void Coordinator::initialize()
{
    //reading coordinator's text file and passing data to nodes
    vector<string> lines = readFile(getBasePath() + "/inputs/coordinator.txt");
    for (int i = 0; i < lines.size(); i++)
    {
        string nodeNumber = lines[i].substr(0, 1);
        std::string outputGate = "toNode" + nodeNumber;
        cMessage *msg = new cMessage(lines[i].c_str());
        send(msg, outputGate.c_str());
    }

    // string msg;
    // msg = modifyMessage("HEY");
    // for (int i = 0; i < msg.size(); i++) {
    //     cout << msg[i] << endl;
    // }
}

void Coordinator::handleMessage(cMessage *msg)
{
}
