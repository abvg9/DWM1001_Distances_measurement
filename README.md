# DWM1001_Distance_measurement

This firmware is intended to be used on the DWM1001-Dev boards to measure distances between them making use of the provided library by Decawave.
[Here](https://www.decawave.com/product/dwm1001-development-board/) you can download all the documentation and software provided by Decawave.

## Introduction
The Decawave library distinguishes between two types of nodes, anchors and tags. Anchors are intended to have a fixed position and tags are free to move. The expected operation is that the tags find out their position thanks to an exchange of messages with the anchors. This architecture greatly limits the possible use of DWM1001 boards when implementing different decentralized systems in which necessarily all nodes can be in motion, for example for autonomous vehicle patrols. This firmware solves this problem, making it possible for all nodes to move freely and obtain the distances between the different nodes.

## Workflow
The operation of this firmware is based on making the nodes go from anchor to tag in an orderly manner, coordinated between the different nodes in order to obtain the different distances between them. The behavior of the firmware can be explained through a state machine.

### Initial State
In the initial state, all nodes are anchors. In this phase, all of them are waiting to detect the rest of the nodes of the network, each time a node is detected, an ordered insertion of the id of this node is made in a list of neighbors. This phase ends when all nodes detect the rest of the network nodes. Once finished, the node gets its id and is inserted neatly into the neighbor list. This state is a hell of a neighbor discovery phase.

![Neighbor discovery phase](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/assets/Neighbor%20discovery%20phase.png)

### States
Once the neighbor discovery phase is finished, each node checks in which mode it should operate (anchor or tag). To find out in which mode they should operate, all they have to do is look at their position in the list of neighbors. Initially, the node that is in position 0 of the neighbor list will be the tag and the rest of the nodes will be the anchors. At this moment, the node that is a tag will wait until it obtains all the distances to the rest of the nodes and the anchors will wait for the tag to finish calculating the distances. When the tag obtains its distances, it will notify the rest of the anchors that it has finished and all of them will advance their status. In such a way that, now the node that is in position 1 of the neighbor list will become the tag. This will repeat itself endlessly.

## Technical aspects

## Limitations
