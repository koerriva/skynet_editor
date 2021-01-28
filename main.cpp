#include <iostream>
#include <raylib.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

using namespace glm;
using namespace spdlog;

void Input(){

}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280,720,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);
    Camera2D camera;
    camera.zoom = 1.0f;
    while(!WindowShouldClose()){
        Input();
        BeginDrawing();
        ClearBackground(DARKBLUE);
        BeginMode2D(camera);
        for (int i = 0; i < 10; ++i) {
            Vector2 center;
            for (int j = 0; j < 10; ++j) {
                center.x = i*20.0f;
                center.y = j*20.0f;
                float radius = 10.0;
                Color color = DARKGREEN;
                DrawCircleV(center,radius,color);
            }
        }
        EndMode2D();
        DrawFPS(5,5);
        DrawText(TextFormat("Mouse Y %2.f",GetMouseWheelMove()),5,20,16,DARKBROWN);
        camera.zoom +=GetMouseWheelMove();
        camera.zoom = clamp<float>(camera.zoom,1,5);
        DrawText(TextFormat("Mouse Zoom %2.f",camera.zoom),5,40,16,DARKBROWN);

        auto mPos = GetMousePosition();
        auto wPos = GetScreenToWorld2D(mPos,camera);

        DrawText(TextFormat("World Pos %2.f,%2.f",wPos.x,wPos.y),5,60,16,DARKBROWN);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
