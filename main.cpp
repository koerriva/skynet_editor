#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

using namespace glm;
using namespace spdlog;

void Input(){

}

bool IsInside(Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    return 100.f>x*x+y*y;
}

int width = 1280,height=720;

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(width,height,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);
    Camera2D camera;
    camera.zoom = 1.0f;
    camera.target = Vector2{0,0};
    float radius = 10.0;
    int row = width/20,col = height/20;
    Vector2 entities[row*col];
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            entities[i*col+j].x = i*radius*2+radius;
            entities[i*col+j].y = j*radius*2+radius;
        }
    }
    while(!WindowShouldClose()){
        auto mPos = GetMousePosition();
        auto wPos = GetScreenToWorld2D(mPos,camera);

        Input();
        BeginDrawing();
        ClearBackground(DARKBLUE);
        BeginMode2D(camera);
        for (auto center : entities) {
            Color color;
            if(IsInside(center,wPos)){
                color = GREEN;
            }else{
                color = DARKGREEN;
            }

            DrawCircleV(center,radius,color);
        }
        EndMode2D();
        DrawFPS(5,5);
        DrawText(TextFormat("Mouse Y %2.f",GetMouseWheelMove()),5,20,16,DARKBROWN);
        camera.zoom +=GetMouseWheelMove();
        camera.zoom = clamp<float>(camera.zoom,1,5);
        DrawText(TextFormat("Mouse Zoom %2.f",camera.zoom),5,40,16,DARKBROWN);
        DrawText(TextFormat("World Pos %2.f,%2.f",wPos.x,wPos.y),5,60,16,DARKBROWN);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
