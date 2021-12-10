#include "Coordinator.h"

#include "helpers.h"

Define_Module(Coordinator);

void Coordinator::initialize()
{
    vector<string> lines;
    lines = readFile("E:/Uni Stuff/Senior II Standing/Computer Networks I/Project/Project-2021-Student/SelectiveRepeatProtocol/src/inputs/coordinator.txt");
    for (int i = 0; i < lines.size(); i++) {
        cout << lines[i] << endl;
    }

    string msg;
    msg = modifyMessage("HEY");
    for (int i = 0; i < msg.size(); i++) {
        cout << msg[i] << endl;
    }

}

void Coordinator::handleMessage(cMessage *msg)
{
}
