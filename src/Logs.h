/*
 * Logs.h
 *
 *  Created on: Dec 9, 2021
 *      Author: fatem
 */

#ifndef LOGS_H_
#define LOGS_H_
#include "helpers.h"
#include <omnetpp.h>
using namespace std;
using namespace omnetpp;

class Logs {

private:
    std::string fileName;          //file name for each pair
    int totalCorrectMessages;      //total number of correct messages
    float totalTransTime;          //increases on every transmission
    float totalTransNum;           //incremented on every transmission
    int nodesReachingEOF;          //number of nodes reaching the end of their input file

public:
    Logs(std::string file);    //constructor takes file name to output in the same file for every pair
    //events: 0-> sends, 1-> received, 2-> drops, 3->timeout
    void addLog(int node, int event, int id, std::string content, float time, bool modified, bool ack, int ackNum); //adds a line in the logs file and prints to console
    void addEOF(int node);         //called when node reaches end of file to print it and calculates statistics if both sides are done
    virtual ~Logs();
};

#endif /* LOGS_H_ */