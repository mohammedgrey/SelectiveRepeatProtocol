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

class Logs
{

private:
    std::string filePath;     // output file path for each pair
    ofstream myfile;          // output file
    int totalCorrectMessages; // total number of correct messages
    double totalTransTime;    // increases on every transmission
    int totalTransNum;        // incremented on every transmission
    int nodesReachingEOF;     // number of nodes reaching the end of their input file
    int startTime;            // to be subtracted from the end time to get the total transmission time
    bool finished;            // for checking if both nodes finished

public:
    Logs(std::string file); // constructor takes file name to output in the same file for every pair
    // events: 0-> sends, 1-> received, 2-> drops, 3->timeout
    void setStartTime(double st);
    void addLog(int node, int event, int id, std::string content, double time, bool modified, bool ack, int ackNum); // adds a line in the logs file and prints to console
    void addEOF(int node);                                                                                           // called when node reaches end of file to print it and calculates statistics if both sides are done
    void incrementTransNum(int n);
    void incrementCorrectMessages(int n);
    void setTransTime(double t);
    void addHammingLog(int errorPos, std::string correctedString); // log for hamming correction
    virtual ~Logs();
};

#endif /* LOGS_H_ */
