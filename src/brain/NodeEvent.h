//
// Created by koerriva on 2021/4/25.
//

#ifndef SKYNET_EDITOR_NODEEVENT_H
#define SKYNET_EDITOR_NODEEVENT_H
#include <qb/event.h>
#include <qb/actor.h>

namespace GamePlay{
struct EActive:public qb::Event{
    int nid;
    int from;
    int to;
    int signal;
};

struct EKill:public qb::Event{
    int signal;
};

class ANode:public qb::Actor{
    const qb::ActorId _id_linked;
public:
    ANode() = delete;
    explicit ANode(const qb::ActorId to):_id_linked(to){}

    bool onInit() final{
        registerEvent<EActive>(*this);
        return true;
    }

    void on(EActive& event){
        if(event.to==id()){
            qb::io::cout() << "ANode id(" << id() << ") received EActive" << std::endl;
            auto& e = push<EActive>(_id_linked);
            e.from = id();
            e.to = _id_linked;
        }
    }

    void on(EKill& event){
        qb::io::cout() << "ANode id(" << id() << ") received EKill" << std::endl;
        kill();
    }
};
}

#endif //SKYNET_EDITOR_NODEEVENT_H
