#ifndef __BITMANUPLATIONS_COORDINATOR_H_
#define __BITMANUPLATIONS_COORDINATOR_H_

#include <omnetpp.h>

#include "helpers.h"

using namespace omnetpp;

class Coordinator : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
