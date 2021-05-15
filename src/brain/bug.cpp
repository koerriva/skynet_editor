//
// Created by koerriva on 2021/5/1.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::UpdateBug() {
        m_BugStop = false;
        m_BugPosition  = Vector3Add(m_BugPosition,Vector3Multiply(m_BugDirection,m_BugVelocity));

        //update velocity
        Vector2 velocity = Vector2Lerp({m_BugVelocity.x,m_BugVelocity.z},{0,0},0.1);
        m_BugVelocity.x = velocity.x;
        m_BugVelocity.z = velocity.y;
    }
    void NodeEditor::StopBug(ActionSignal actionSignal){
        if(actionSignal.type==0&&!m_BugStop){
            TraceLog(LOG_INFO, TextFormat("Bug::Stop"));
            m_BugStop = true;
            m_BugVelocity = {0,0,0};
        }
    }
    void NodeEditor::MoveBug(ActionSignal actionSignal){
        if(actionSignal.type==1&&!m_BugStop){
            m_BugVelocity.x = 0.02;
            m_BugVelocity.z = 0.02;
        }
    }
    void NodeEditor::JumpBug(ActionSignal actionSignal){
        if(actionSignal.type==2&&!m_BugStop&&!m_BugJumping){
            TraceLog(LOG_INFO, TextFormat("Bug::BeginJump"));
            m_BugJumping = true;
        }
    }
    void NodeEditor::TurnBug(ActionSignal actionSignal){
        float angle = 0;
        float unitDeg = 0;
        if(actionSignal.type==3&&!m_BugStop){
            //左转
            unitDeg = 1.0f/10.0f;
            angle = -unitDeg*PI/180.f;
        }
        if(actionSignal.type==4&&!m_BugStop){
            //右转
            unitDeg = 1.0f/10.0f;
            angle = unitDeg*PI/180.f;
        }
        m_BugRotation += unitDeg;
        m_BugDirection = Vector3Transform(m_BugDirection,MatrixRotateY(angle));
    }
    void NodeEditor::PlayBugAnimation(Animation& animation,bool isLoop){
        if (animation.isOver()&&isLoop) {
            animation.frameCounter = 0;
        }
        animation.frameCounter++;
        UpdateModelAnimation(m_Bug,animation.data,animation.frameCounter);
    }
}