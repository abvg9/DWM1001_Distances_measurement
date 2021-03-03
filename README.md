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
Once the neighbor discovery phase is finished, each node checks in which mode it should operate (anchor or tag). To find out in which mode they should operate, all they have to do is look at their position in the list of neighbors. Initially, the node that is in position 0 of the neighbor list will be the tag and the rest of the nodes will be the anchors. At this moment, the node that is a tag will wait until it obtains all the distances to the rest of the nodes and the anchors will wait for the tag to finish calculating the distances. When the tag obtains its distances, it will notify the rest of the anchors that it has finished and all of them will advance their status. In such a way that, now the node that is in position 1 of the neighbor list will become the tag. This will repeat itself endlessly. In the following images a series of transitions of the state machine will be shown.

![State 0](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/assets/State%200.png)

![State 1](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/assets/State%201.png)

![State 2](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/assets/State%202.png)

### Algorithm aspects
If you look at the previous images, you can see that all the nodes of the network can find out the distances to the rest of the nodes in 3 changes of states. If we extrapolate this case to a case where there are n nodes, we know that the network will take time to know all the distances in n-1 transitions of the state machine.

![Demostration of the algorithm cost](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/assets/Standar%20method%20formula.gif)

The second term of the equation is the total edges calculation in a complete graph.

## Technical aspects
There are certain technical aspects that can be useful if you want to make modifications to this software to better suit your project or to have a deep understanding of the firmware.

* When a node is a tag, it will return the distances to the rest of the nodes through the UART port. Returning the identifier of the node and the distance to it.
* When a node changes from tag to anchor or vice versa, we need to reset the board. This implies that we necessarily need to save information in non-volatile storage, such as non-volatile memory. The dwm library provides us with mechanisms to write and read from it and what this firmware does is add an abstraction layer to manipulate the non-volatile memory in a finer grain and store information about the current state of the state machine. The following information is stored in the NVM:
** Number of neighbors in the network.
** Index of the node that has to be a tag.
** Index of the node in the neighbor list.
** Index of the anchor node that should initiate communications with the rest.
** Boolean that indicates if in the last state the node was tag.
** Identifiers of the neighbors the node have scanned.
* If you want to know more information about the NVM and the different variables stored in it to make the state machine work, it is recommended to see the nvm.c and nvm.h files for a better understanding.
* To send messages between the nodes, this firmware takes advantage of the proper sending of messages that the dwm library does to send information. This is due to the fact that dwm has the capabilities of sending messages from one node to another. The way to send messages is through indicating positions to the node. When a node wants to send a message to others, it must write in its position register the value that it wants others to read, specifically, it must modify the X position.

## Limitations
