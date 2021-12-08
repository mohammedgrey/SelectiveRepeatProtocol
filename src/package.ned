package selectiverepeatprotocol;

@license(LGPL);

network Network
{
    parameters:
        double timeoutSeconds;
        double delaySeconds;
        int windowSize;
    
    @display("bgb=326,159");
    submodules:
        coordinator: Coordinator;
        node0: Node;
        node1: Node;
        node2: Node;
        node3: Node;
        node4: Node;
        node5: Node;
    connections:
        //connet the coordinator to each node
        coordinator.toNode0 --> node0.coordinatorInput;
        coordinator.toNode1 --> node1.coordinatorInput;
        coordinator.toNode2 --> node2.coordinatorInput;
        coordinator.toNode3 --> node3.coordinatorInput;
        coordinator.toNode4 --> node4.coordinatorInput;
        coordinator.toNode5 --> node5.coordinatorInput;
        //connect pairs of nodes together
        //pair 0-1
        node0.peerLink <--> node1.peerLink;
        //pair 2-3
        node2.peerLink <--> node3.peerLink;
        //pair 4-5
        node4.peerLink <--> node5.peerLink;
}
