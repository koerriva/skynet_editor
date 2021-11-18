//
// Created by 50409 on 2021/11/18.
//

#include <cassert>
#include <cstdio>
#include <cstring>
#include "network.h"
#include "uuid.hpp"

namespace GamePlay{
    const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::string generateUUID(){
        std::string uuid = std::string(36,' ');
        int rnd = 0;
        int r = 0;

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';

        uuid[14] = '4';

        for(int i=0;i<36;i++){
            if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
                if (rnd <= 0x02) {
                    rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
                }
                rnd >>= 4;
                uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
            }
        }
        return uuid;
    }

    bool Peer::Pub(const char *msg) {
        printf("发送 %s\n",msg);
        int r = zmq_send(publisher, uuid.data(), uuid.size(), ZMQ_SNDMORE);
        r += zmq_send(publisher, msg, strlen(msg), 1);
        return r > 0;
    }

    std::string Peer::Sub(const char *topic) {
        zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));

        char addr[45] = {0};
        char buffer[45] = {0};
        zmq_recv(subscriber,addr, strlen(addr)-1,1);
        int r= zmq_recv(subscriber,buffer, strlen(buffer)-1,1);
        if(r>0){
            printf("接收 %s\n",buffer);
        }else{
//            buffer[0] = '0';
        }

        return std::string{buffer};
    }

    Peer::Peer() {
        uuid = generateUUID();
        uuid = utility::uuid::generate();
        context = zmq_ctx_new();

        if (publisher == nullptr) {
            printf("发布端点 %s\n", uuid.data());
            publisher = zmq_socket(context, ZMQ_PUB);
            int r = zmq_connect(publisher, "tcp://localhost:5555");
            int color = r == 0 ? 102 : 101;
            printf("连接到代理服务器 \033[%dm %s\033[m\n", color, r == 0 ? "ok" : "err");
            assert(r == 0);
        }

        if (subscriber== nullptr){
            subscriber = zmq_socket(context, ZMQ_SUB);
            int r = zmq_connect(subscriber, "tcp://localhost:5556");
            int color = r == 0 ? 102 : 101;
            printf("连接到代理服务器 \033[%dm %s\033[m\n", color, r == 0 ? "ok" : "err");
            assert(r == 0);
        }
    }

    Peer::Peer(Peer &&other) noexcept {
        {
            uuid = other.uuid;
            context = other.context;

            publisher = other.publisher;
            subscriber = other.subscriber;
        }
    }
}