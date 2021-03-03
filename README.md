# DWM1001_Distance_measurement

This firmware is intended to be used on the DWM1001-Dev boards to measure distances between them making use of the provided library by Decawave.
[Here](https://www.decawave.com/product/dwm1001-development-board/) you can download all the documentation and software provided by Decawave.
In the repository there is a project that is designed to run in the SEGGER development environment. In order to use it, you must modify the build_tool_chain_directory variable and set the path to the compiler on your computer. If you do not understand anything that is being explained, we recommend to go to the Decawave documentation and read how to use the SEGGGER development environment and how to run decawave onboard examples.

## Introduction
The Decawave library distinguishes between two types of nodes, anchors and tags. Anchors are intended to have a fixed position and tags are free to move. The expected operation is that the tags find out their position thanks to an exchange of messages with the anchors. This architecture greatly limits the possible use of DWM1001 boards when implementing different decentralized systems in which necessarily all nodes can be in motion, for example for autonomous vehicle patrols. This firmware solves this problem, making it possible for all nodes to move freely and obtain the distances between the different nodes.

## Workflow
The operation of this firmware is based on making the nodes go from anchor to tag in an orderly manner, coordinated between the different nodes in order to obtain the different distances between them. The behavior of the firmware can be explained through a state machine.

### Initial State
In the initial state, all nodes are anchors. In this phase, all of them are waiting to detect the rest of the nodes of the network, each time a node is detected, an ordered insertion of the id of this node is made in a list of neighbors. This phase ends when all nodes detect the rest of the network nodes. Once finished, the node gets its id and is inserted neatly into the neighbor list. This state is called neighbor discovery phase.

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
Number of neighbors in the network, index of the node that has to be a tag, index of the node in the neighbor list, index of the anchor node that should initiate communications with the rest, boolean that indicates if in the last state the node was tag and identifiers of the neighbors the node have scanned.
* If you want to know more information about the NVM and the different variables stored in it to make the state machine work, it is recommended to see the [nvm.c](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/source/nvm/nvm.c) and [nvm.h](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/header/nvm/nvm.h) files for a better understanding.
* To send messages between the nodes, this firmware takes advantage of the proper sending of messages that the dwm library does to send information. This is due to the fact that dwm library does not allow messages to be sent from one node to another. The way to send messages is through indicating positions to the node. When a node wants to send a message to others, it must write in its position register the value that it wants others to read, specifically, it must modify the X position. This message sending mechanism only works for anchors, that is, if the node is anchor, it can be used, but being tag, messages cannot be sent to other nodes.
```c++
typedef enum {
  clean_message_buffer,
  net_nodes_finded,
  tag_got_distances
} message_type;
```
```c++
bool send_message(message_type m) {
  dwm_pos_t message = {m , 0, 0, 0};
  return err_check(dwm_pos_set(&message));
}
```
* This firmware is designed to work even if the boards are turned off and on again. The nodes will maintain the state before the shutdown and will continue working.
* When the board has a permanent green led on, it will mean that said node is a tag, if it has a permanently on blue led it will mean that it is anchor. When the nodes have LEDs flashing, it means that they are waiting to pass state.
* For more information look at the code, it is fully documented.

## Limitations
* The network must have a number of fixed nodes. This value is determined in the [common.h](https://github.com/UCM-237/DWM1001_Distances_measurement/blob/main/header/common.h) file, in the NET_NUM_NODES variable.
```c++
#define NET_NUM_NODES 4
```
* The network can only have DWM_RANGING_ANCHOR_CNT_MAX (14) nodes.
* If for any reason, one of the nodes of the network falls, the others will end up returning to the initial state, waiting for it to return, but will remain permanently in that state until it returns.
* All nodes must be within 200 meters, which is the maximum distance at which one DWM1001 board can detect another.
* If you want to modify the number of nodes in the network, you will have to modify the variable NET_NUM_NODES, erase all the memory and load the program again on all nodes.
* Whenever you use a node on a new network, it must erase all the memory of the device. If not, the nvm of the device will not be updated with that of a new network and it will not work.

## Future releases
After considering it, we have believed that it is best to finish the development of this project and start a new one, in such a way that we will no longer use the dwm library and we will create our own. The [new proyect](https://github.com/UCM-237/DWM1001_Distance_measurement_v2) will implement what this firmware has in addition to an improved system in which obtaining distances will be twice as fast. We hope to have it before the end of 2021.

## License
[MIT](https://choosealicense.com/licenses/mit/)
