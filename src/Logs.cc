/*
 * Logs.cpp
 *
 *  Created on: Dec 9, 2021
 *      Author: fatem
 */

#include "Logs.h"

Logs::Logs(std::string file) {
    // TODO Auto-generated constructor stub
    filePath= getBasePath() + "/outputs/"+file;
    nodesReachingEOF=0;
    totalTransTime=0;
    totalTransNum=0;
}

//events: 0-> sends, 1-> received, 2-> drops, 3->timeout
void Logs::addLog(int node, int event, int id, std::string content, double time, bool modified, bool ack, int ackNum) {

    //constructing log
    std::string log="";
    switch(event) {
        case 0:
            log="node"+std::to_string(node)+" sends message with id="+std::to_string(id)+" and content= ''"+content+"'' at "+std::to_string(time);
            if(modified) log+=" with modification, ";
            if(ack) log+="and piggybacking ACK number= "+std::to_string(ackNum);
            else log+="and NACK number= "+std::to_string(ackNum);
            totalTransNum++;    //increment number of transmissions on every sent message
            break;
        case 1:
            log="node"+std::to_string(node)+" received message with id="+std::to_string(id)+" and content= ''"+content+"'' at "+std::to_string(time);
            if(modified) log+=" with modification,";
            if(ack) log+=" and piggybacking ACK number= "+std::to_string(ackNum);
            else log+=" and NACK number= "+std::to_string(ackNum);
            break;
        case 2:
            log="node"+std::to_string(node)+" drops message with id="+std::to_string(id);
            break;
        case 3:
            log="node"+std::to_string(node)+" timeout for message with id="+std::to_string(id)+" at t= "+std::to_string(time);
            break;
    }

    //printing log to console
    cout<< log<< endl;

    //adding log to output file
    ofstream myfile;
    myfile.open(filePath, std::ios_base::app);
    myfile<< log<<endl;
    myfile.close();
}

void Logs::addEOF(int node) {

    nodesReachingEOF++;
    cout<< "node"<< node << " end of input file" << endl;

    ofstream myfile;
    myfile.open(filePath, std::ios_base::app);
    myfile<< "node"<< node << " end of input file" << endl;

    //if the pair finished their input files
    if(nodesReachingEOF==2){

        float throughput= totalCorrectMessages/totalTransTime;

        //printing to console
        cout<< "Total transmission time= "<< totalTransTime<<endl;
        cout<< "Total number of transmissions= "<< totalTransNum<<endl;
        cout<< "The network throughput= "<< throughput<<endl;

        //appending to output file

        myfile<< "Total transmission time= "<< totalTransTime<<endl;
        myfile<< "Total number of transmissions= "<< totalTransNum<<endl;
        myfile<< "The network throughput= "<< throughput<<endl;
    }
    myfile.close();
}

Logs::~Logs() {
    // TODO Auto-generated destructor stub
}

