//
// Created by 杨玉印 on 2021/11/4.
//

#ifndef SKYNET_EDITOR_NETWORKMANAGER_H
#define SKYNET_EDITOR_NETWORKMANAGER_H

#include <enet/enet.h>
#include <raylib.h>
#include <vector>

namespace Engine{
    class NetworkManager {
    public:
        static void Init();

    private:
        NetworkManager(){
            if(enet_initialize()!=0){
                TraceLog(LOG_ERROR,"An error occurred while initializing ENet.");
                exit(EXIT_FAILURE);
            }

            enet_address_set_host(&m_ServerAddress,"0.0.0.0");
            m_ServerAddress.port = 27998;

            m_Server = enet_host_create(&m_ServerAddress,1024,2,0,0);
            if(m_Server == nullptr){
                TraceLog(LOG_ERROR,"An error occurred while trying to create an ENet server host.");
                exit(EXIT_FAILURE);
            }
            TraceLog(LOG_INFO,"Server Listen at 0.0.0.0:27998");

            m_Client = enet_host_create(nullptr,1024,2,0,0);
            if(m_Client == nullptr){
                TraceLog(LOG_ERROR,"An error occurred while trying to create an ENet client host.");
                exit(EXIT_FAILURE);
            }
        }

        ~NetworkManager(){
            enet_deinitialize();
        }

        void UpdateServer(uint32_t time);

        void SendMessage(char* host,uint32_t time);

        ENetAddress m_ServerAddress;
        ENetHost* m_Server = nullptr;
        std::vector<ENetPeer *> m_InPeer;
        std::vector<ENetPeer *> m_OutPeer;
        static NetworkManager* s_Instance;
    };
}


#endif //SKYNET_EDITOR_NETWORKMANAGER_H
