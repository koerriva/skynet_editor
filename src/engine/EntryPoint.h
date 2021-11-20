//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_ENTRYPOINT_H
#define SKYNET_EDITOR_ENTRYPOINT_H

#include <unistd.h>
#include <random>
#include "Log.h"
#include "Application.h"
#include "thread"
#include "zmq.h"
#include "signal.h"

#include "machineid/machineid.h"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

//  ---------------------------------------------------------------------
//  消息处理
//
//  程序开始运行时调用s_catch_signals()函数；
//  在循环中判断s_interrupted是否为1，是则跳出循环；
//  很适用于zmq_poll()。

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals ()
{
    signal(SIGINT,s_signal_handler);
    signal(SIGTERM,s_signal_handler);
}

int main(int argc,char** argv){
#ifdef _WIN32
    system("chcp 65001");
#endif
    std::string machine_hash = machineid::machineHash();
    printf("MachineId [%s]\n",machine_hash.data());
    const char* peer_type = argv[1];
    printf("节点类型 %s\n",peer_type);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,100);

    if(strcmp(argv[1],"broker") == 0){
        //准备上下文和套接字
        printf("准备上下文和套接字\n");
        void *context = zmq_init(5);
        void *frontend = zmq_socket(context, ZMQ_ROUTER);
        void *backend  = zmq_socket(context, ZMQ_DEALER);
        zmq_bind(frontend, "tcp://*:5555");
        zmq_bind(backend,  "tcp://*:5556");

        //在套接字间转发消息
        printf("监听中 ...\n");
        zmq_proxy(frontend,backend, nullptr);
        //程序不会运行到这里，不过还是做好清理工作
        zmq_close (frontend);
        zmq_close (backend);
        zmq_term (context);
        return 0;
    }else if(strcmp(argv[1],"proxy") == 0){
        //准备上下文和套接字
        void *context = zmq_init(5);
        void *frontend = zmq_socket(context, ZMQ_XSUB);
        void *backend  = zmq_socket(context, ZMQ_XPUB);
        zmq_bind(frontend, "tcp://*:5555");
        zmq_bind(backend,  "tcp://*:5556");

        //在套接字间转发消息
        printf("监听中 ...\n");
        zmq_proxy(frontend,backend, nullptr);

        //程序不会运行到这里，不过还是做好清理工作
        zmq_close(frontend);
        zmq_close(backend);
        zmq_term(context);
    }else if(strcmp(argv[1],"client") == 0){
        const char* id = argv[2];
        const char* other = argv[3];

        printf("连接到代理服务器 5555...\n");
        void *context = zmq_ctx_new();
        void *req = zmq_socket(context, ZMQ_REQ);

        printf("客户节点 %s, 业务节点 %s\n",id,other);

        zmq_setsockopt(req,ZMQ_IDENTITY,id,strlen(id));
        const char* addr = "tcp://localhost:5555";
        int rc = zmq_connect(req, addr);
        int color = rc==0?102:101;
        printf("连接到代理服务器 %s \033[%dm %d\033[m\n",addr,color,rc);
        assert(rc==0);

        while(s_interrupted==0){
            char buffer[64] = {0};
            sprintf(buffer,"%d",distribution(generator));
            printf("send %s\n",buffer);
            zmq_send(req,buffer,strlen(buffer),0);

            memset(buffer,0,64);
            zmq_recv(req,buffer,63,0);
            printf("recv %s\n",buffer);

            sleep(1);
        }

        zmq_close(req);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"service") == 0){
        const char* id = argv[2];
        void *context = zmq_ctx_new();
        void *rep = zmq_socket(context, ZMQ_REP);

        printf("服务节点 %s\n",id);

//        zmq_setsockopt(rep,ZMQ_IDENTITY,id,strlen(id));
        const char* addr = "tcp://localhost:5556";
        int rc = zmq_connect(rep, addr);
        int color = rc==0?102:101;
        printf("连接到代理服务器 %s \033[%dm %d\033[m\n",addr,color,rc);
        assert(rc==0);
        while(s_interrupted==0){
            char buffer[64] = {0};
            zmq_recv(rep, buffer, 63, 0);
            printf("recv %s\n",buffer);
//            sleep (1);          //  Do some 'work'
            zmq_send(rep, "1", 1, 0);
        }
        zmq_close(rep);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"pub") == 0){
        const char* id = argv[2];
        void *context = zmq_ctx_new();
        void *publisher = zmq_socket(context, ZMQ_PUB);

        printf("发布者 %s\n",id);

        const char* addr = "tcp://localhost:5555";
        int rc = zmq_connect(publisher,addr);
        int color = rc==0?102:101;
        printf("连接到代理服务器 %s \033[%dm %d\033[m\n",addr,color,rc);
        assert(rc==0);

        while (s_interrupted==0){
            char buffer[10] = {0};
            zmq_send(publisher,id,strlen(id),ZMQ_SNDMORE);
            sprintf(buffer,"%d",distribution(generator));
            zmq_send(publisher,buffer,strlen(buffer),0);
            printf("Publish %s\n",buffer);
            sleep(1);
        }
        zmq_close(publisher);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"sub") == 0){
        const char* id = argv[2];
        const char* other = argv[3];

        printf("连接到代理服务器 5556...\n");
        void *context = zmq_ctx_new();
        void *subscriber = zmq_socket(context, ZMQ_SUB);

        printf("订阅节点 %s, 发布节点 %s\n",id,other);

        zmq_setsockopt(subscriber,ZMQ_SUBSCRIBE,other,strlen(other));
        const char* addr = "tcp://localhost:5556";
        int rc = zmq_connect(subscriber, addr);
        int color = rc==0?102:101;
        printf("连接到代理服务器 %s \033[%dm %d\033[m\n",addr,color,rc);
        assert(rc==0);

        while(s_interrupted==0){
            char addr[10] = {0};
            char buffer[10] = {0};
            zmq_recv(subscriber,addr,9,0);
            zmq_recv(subscriber,buffer,9,0);
            printf("Recv %s\n",buffer);
        }
        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"pub-sub") == 0){
        const char* id = argv[2];
        const char* other = argv[3];

        printf("发布节点 %s, 订阅节点 %s\n",id,other);

        void *context = zmq_ctx_new();

        void *publisher = zmq_socket(context, ZMQ_PUB);
        zmq_connect(publisher,"tcp://localhost:5555");

        void *subscriber = zmq_socket(context, ZMQ_SUB);
        zmq_setsockopt(subscriber,ZMQ_SUBSCRIBE,other,strlen(other));
        zmq_connect(subscriber,"tcp://localhost:5556");

        while(s_interrupted==0){
            {
                char buffer[10] = {0};
                zmq_send(publisher,id,strlen(id),ZMQ_SNDMORE);
                sprintf(buffer,"%d",distribution(generator));
                zmq_send(publisher,buffer,strlen(buffer),0);
                printf("Publish %s\n",buffer);
            }

            {
                char addr[10] = {0};
                char buffer[10] = {0};
                zmq_recv(subscriber,addr,9,1);
                zmq_recv(subscriber,buffer,9,1);
                printf("Recv %s\n",buffer);
            }

            sleep(1);
        }

        zmq_close(subscriber);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"register") == 0){
        const char* id = argv[2];

        printf("注册节点 %s\n",id);

        void *context = zmq_ctx_new();

        void *publisher = zmq_socket(context, ZMQ_PUB);
        int r = zmq_bind(publisher,"pgm://192.168.100.142;238.1.1.1:5000");
        assert(r==0);

        const char* addr = "localhost:5555";
        while(s_interrupted==0){
            zmq_send(publisher,"broad",5,ZMQ_SNDMORE);
            zmq_send(publisher,addr,strlen(addr),0);
            printf("Broadcast %s\n",addr);
            sleep(1);
        }

        zmq_close(publisher);
        zmq_ctx_destroy(context);
        return 0;
    }else if(strcmp(argv[1],"node") == 0){
        const char* id = argv[2];

        printf("测试节点 %s\n",id);

        void *context = zmq_ctx_new();

        void *req = zmq_socket(context, ZMQ_SUB);
        zmq_setsockopt(req,ZMQ_SUBSCRIBE,"broad",5);
        int r = zmq_connect(req,"pgm://192.168.100.142;238.1.1.1:5557");
        assert(r==0);

        while(s_interrupted==0){
//            zmq_send(req,id, strlen(id),0);
            char buffer[255] = {0};
            zmq_recv(req,buffer,254,0);
            printf("Register %s\n",buffer);

//            sleep(1);
        }

        zmq_close(req);
        zmq_ctx_destroy(context);
        return 0;
    }

    Engine::Log::Init();

    auto app = Engine::Create();
    std::thread async_worker(Engine::AsyncRun,app);
    app->Run();
    async_worker.join();
    delete app;
    return 0;
}

#endif //SKYNET_EDITOR_ENTRYPOINT_H
