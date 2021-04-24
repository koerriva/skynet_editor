#include <brain/NodeEditor.h>
#include "engine/Engine.h"

class EditorLayer: public Engine::Layer{
public:
    EditorLayer(Camera2D camera){
        m_NodeEditor = new GamePlay::NodeEditor();
        m_Camera = camera;
    }

    void OnAttach() override{
        m_NodeEditor->Load(m_Camera);
    }

    void OnDeAttach() override{
        TraceLog(LOG_INFO,"EditorLayer Exit!");
        m_NodeEditor->Save();
    }

    void OnUpdate() override{
        m_NodeEditor->Update();
    }

    void OnGUIRender() override{
        m_NodeEditor->Show();
    }

private:
    GamePlay::NodeEditor* m_NodeEditor;
    Camera2D m_Camera;
};

class Game: public Engine::Application{
public:
    Game(){
//        LoadFont();

        m_EditorLayer = new EditorLayer(m_Camera2D);
        PushLayer(m_EditorLayer);
    }
    ~Game() override{
        PopLayer(m_EditorLayer);
    }

private:
    void LoadFont(){
        //基本汉字 4E00-9FA5
        int charsCount = 0x9FA5+1;
        int* fontChars = (int*)MemAlloc(sizeof(int)*charsCount);
        TraceLog(LOG_INFO,"Init SC Font Memory");
        for (int i = 0; i < charsCount; ++i) {
            fontChars[i] = i;
        }
        TraceLog(LOG_INFO,"Load SC Font");
        m_Font = LoadFontEx("data/font/NotoSansSC-Regular.otf",16,fontChars,charsCount);
//        m_Font = LoadFontEx("data/font/LiHeiPro.ttf",16,fontChars,charsCount);
        TraceLog(LOG_INFO,"SC Font Load");
        MemFree(fontChars);
    }

private:
    Font m_Font;
    EditorLayer* m_EditorLayer;
};

Engine::Application* Engine::Create(){
    return new Game();
}