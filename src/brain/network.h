//
// Created by 50409 on 2021/11/18.
//

#ifndef SKYNET_EDITOR_NETWORK_H
#define SKYNET_EDITOR_NETWORK_H

#include <string>
#include "zmq.h"

namespace GamePlay {
    std::string generateUUID();

    struct Peer {
        std::string uuid{};

        Peer();

        Peer(Peer&& other) noexcept;

        bool Pub(const char *msg);
        bool Sub(const char *topic);

        std::string Recipes();

        ~Peer(){
            printf("\033[101m Peer[%s] Destroy \033[m\n",uuid.data());
            if(publisher){
                zmq_close(publisher);
            }
            if(subscriber){
                zmq_close(subscriber);
            }
            if(context){
                zmq_ctx_destroy(context);
            }
        }

    private:
        void *context = nullptr;
        void *publisher = nullptr;
        void *subscriber = nullptr;

        const char* pub_addr = "tcp://localhost:5555";
        const char* sub_addr = "tcp://localhost:5556";
    };
}

#endif //SKYNET_EDITOR_NETWORK_H
