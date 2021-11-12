//
// Created by 杨玉印 on 2021/11/4.
//

#include "NetworkManager.h"

namespace Engine{
    NetworkManager* NetworkManager::s_Instance = nullptr;

    void NetworkManager::Init() {
        if(s_Instance== nullptr){
            s_Instance = new NetworkManager();
        }
    }

    void NetworkManager::UpdateServer(uint32_t time) {
        ENetEvent eNetEvent;
        while (enet_host_service(m_Server,&eNetEvent,time)>0){
            switch (eNetEvent.type) {
                //建立连接
                case ENET_EVENT_TYPE_CONNECT:
                {
                    auto host = eNetEvent.peer->address.host;
                    auto port = eNetEvent.peer->address.port;
                    TraceLog(LOG_INFO,TextFormat("Connect Peer.Host %d,Peer.Port %d",host,port));

                    m_InPeer.push_back(eNetEvent.peer);
                }break;
                case ENET_EVENT_TYPE_NONE:{
                }break;
                //收到信息
                case ENET_EVENT_TYPE_RECEIVE:{
                }break;
                //断开连接
                case ENET_EVENT_TYPE_DISCONNECT:{
                    auto host = eNetEvent.peer->address.host;
                    auto port = eNetEvent.peer->address.port;
                    auto it = std::find(m_InPeer.begin(),m_InPeer.end(),eNetEvent.peer);
                    if(it!=m_InPeer.end()){
                        m_InPeer.erase(it);
                        TraceLog(LOG_INFO,TextFormat("Disconnect Peer.Host %d,Peer.Port %d",host,port));
                    }
                }break;
            }
        }
    }

    void NetworkManager::SendMessage(char *host,uint32_t time) {
        ENetHost* client = enet_host_create(nullptr,1,1,0,0);
        ENetAddress address;
        enet_address_set_host(&address,host);
        address.port = 27998;
        ENetPeer* peer = enet_host_connect(client,&address,2,0);

        ENetEvent * event;
        while (enet_host_service(client,event,time)>0){

        }
    }
}
