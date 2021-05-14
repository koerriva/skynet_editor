//
// Created by 50409 on 2021/5/13.
//

#ifndef SKYNET_EDITOR_GRAPHEDITOR_H
#define SKYNET_EDITOR_GRAPHEDITOR_H


#include <raylib.h>

namespace GamePlay{
    class GraphEditor {

    };

    struct GraphNodeSlot{

    };

    struct GraphNode{
        unsigned long id;
        Vector2 position{};
        float radius=16;

        //mouse event
        bool cursorIn = false;
        bool cursorOut = false;
        //editor
        int editType = 0;
        int editColorType = 0;
        bool editColorMode = false;
        Color color{};

        int parent;
        Vector2 pinPosition={};
        int children[8] = {};
        int linkFrom=0;
        int linkId=0;

        GraphNode(){}
        GraphNode(unsigned long id):id(id){}

        void CursorIn(){ cursorIn = true;cursorOut= false;}
        void CursorOut() {cursorIn = false;cursorOut = true;}
    };
}


#endif //SKYNET_EDITOR_GRAPHEDITOR_H
