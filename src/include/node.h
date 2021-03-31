//
// Created by 杨玉印 on 2021/3/31.
//

#ifndef SKYNET_EDITOR_NODE_H
#define SKYNET_EDITOR_NODE_H

struct NodeId{
    unsigned value;
    void* addr;
};

struct Node {
    NodeId id;
    int data;
};

struct NodeLink {
    NodeId id;
    NodeId input[1024];
    NodeId output[1024];
};

#endif //SKYNET_EDITOR_NODE_H
