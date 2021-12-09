#include "Coordinator.h"

Define_Module(Coordinator);
#include <map>


void Coordinator::initialize()
{
    map<string, string> coordinatorFilePath;
    //TODO: add the getenv("HOSTNAME") of your pc and the corresponding full file path
    coordinatorFilePath["DESKTOP-LA84EMV"]="C:/omnetpp-5.7/samples/SelectiveRepeatProtocol/src/inputs/coordinator.txt";

    vector<string> lines = readFile(coordinatorFilePath[getenv("HOSTNAME")]);
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
