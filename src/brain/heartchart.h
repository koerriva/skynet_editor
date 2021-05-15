//
// Created by 50409 on 2021/5/15.
//

#ifndef SKYNET_EDITOR_HEARTCHART_H
#define SKYNET_EDITOR_HEARTCHART_H

#include "raylib.h"
#include "vector"
#include "deque"

namespace GamePlay{
    class HeartChart{
    private:
        int currentIndex = 0;
        int currentSignal = 0;
        Texture2D texture2D{};
        Color *pixels;
        int width=120,height=60;
        int data[120]{};
    public:
        HeartChart(){
            pixels = (Color *)RL_CALLOC(width*height, sizeof(Color));
            texture2D = LoadTextureFromImage(GenImageColor(width,height,SHENHAILV));
        }

        virtual ~HeartChart() {
            delete pixels;
        }

        void OnReceiveData(int signal){
            currentSignal = signal;
            data[currentIndex++] = currentSignal;
            if(currentIndex>119)currentIndex=0;
        }

        void Render(Camera camera,Vector3 center){
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    pixels[y*width+x] = SHENHAILV;

                    int idx = x;
                    int signal = data[idx];
                    int h = signal==0?55:5;
                    pixels[(h-1)*width+x] = RED;
                    if(currentIndex==idx){
                        pixels[y*width+x] = DARKBLUE;
                    }
                    int afterSignal = data[idx>118?0:idx+1];
                    if(signal!=afterSignal){
                        if(y>=5&&y<=55){
                            pixels[y*width+x] = RED;
                        }
                    }
                }
            }
            UpdateTexture(texture2D,pixels);
            DrawBillboard(camera,texture2D,center,4,WHITE);
        }

        void Update(){
        }
    };
}

#endif //SKYNET_EDITOR_HEARTCHART_H
