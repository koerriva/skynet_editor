//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_ENTRYPOINT_H
#define SKYNET_EDITOR_ENTRYPOINT_H

#include <unistd.h>
#include "Log.h"
#include "Application.h"
#include "thread"

#include "zmq.h"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc,char** argv){
#ifdef _WIN32
    system("chcp 65001");
#endif
    printf("%d %s\n",argc,argv[1]);

    const char* data_topic = "/node/";
    if(strcmp(argv[1],"server") == 0){
        //  Socket to talk to clients
        void *context = zmq_ctx_new ();
        void *responder = zmq_socket (context, ZMQ_PUB);
        int rc = zmq_bind (responder, "tcp://*:5555");
        assert (rc == 0);

        while (true) {
            zmq_send(responder, "/node/1", 7, ZMQ_SNDMORE);
            zmq_send(responder, "{'id':1,'s':98}",15,0);
            printf("Send Signal...\n");
            sleep (1);          //  Do some 'work'
        }
        return 0;
    }else if(strcmp(argv[1],"client") == 0){
        printf ("Connecting to server…\n");
        void *context = zmq_ctx_new ();
        void *requester = zmq_socket (context, ZMQ_REQ);
        zmq_connect(requester, "tcp://localhost:5555");
        const char* id = argv[2];

        char* other;
        if(strcmp(id,"yyy")==0){
            other = "xjb";
        }else{
            other = "yyy";
        }

        printf("Register peer %s\n",id);

        char cmd_buffer[512] = {0};
        sprintf(cmd_buffer,"{'type':'C1001','data':{'id':'%s'}}",id);
        zmq_send(requester,cmd_buffer, strlen(cmd_buffer),0);
        memset(cmd_buffer,0,512);
        zmq_recv(requester,cmd_buffer,511,0);

        if(strcmp(cmd_buffer,"OK")==0){
            printf("Register OK!\n");
            memset(cmd_buffer,0,512);

            int request_nbr;
            for (request_nbr = 0; request_nbr != 10; request_nbr++) {
                int p = GetRandomValue(1,100);
                if(p>60){
                    sprintf(cmd_buffer,"{'type':'C1002','data':{'from':'%s','to':'%s','s':%d}}",id,other,p);
                    zmq_send(requester,cmd_buffer,strlen(cmd_buffer),0);
                    printf("Sending %s\n", cmd_buffer);
                    memset(cmd_buffer,0,512);
                    zmq_recv(requester,cmd_buffer,511,0);
                    printf("Receiving %s\n", cmd_buffer);
                }else{
                    zmq_recv(requester,cmd_buffer,511,0);
                    printf("Listening ... %s\n",cmd_buffer);
                }

                sleep(1);

                memset(cmd_buffer,0,512);
            }
        }else{
            printf("Register FAIL!\n");
        }
        zmq_close (requester);
        zmq_ctx_destroy (context);
        return 0;
    }else{
        printf("Connecting to server…\n");
        void *context = zmq_ctx_new ();
        void *requester = zmq_socket (context, ZMQ_SUB);
        zmq_connect(requester, "tcp://localhost:5555");

        const char* id = argv[2];
        char channel[512] = {0};
        sprintf(channel,"/node/%s",id);
        printf("channel len %d\n", strlen(channel));
        zmq_setsockopt(requester,ZMQ_SUBSCRIBE,channel, strlen(channel));

        int request_nbr;
        for (request_nbr = 0; request_nbr != 10; request_nbr++) {
            char buffer [512] = {0};
//            printf ("Sending Hello %d…\n", request_nbr);
//            zmq_send (requester, "Hello", 5, 0);
            zmq_recv (requester, buffer, 511, 0);
            printf("Received World %s %d\n", buffer, request_nbr);
        }
        zmq_close (requester);
        zmq_ctx_destroy (context);
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
