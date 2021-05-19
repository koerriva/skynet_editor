#include <brain/NodeEditor.h>
#include "engine/Engine.h"

class EditorLayer: public Engine::Layer{
public:
    EditorLayer(Font font){
        m_NodeEditor = new GamePlay::NodeEditor();
        m_Font = font;
    }

    void OnAttach() override{
        m_NodeEditor->Load(m_Font);
    }

    void OnDeAttach() override{
        TraceLog(LOG_INFO,"EditorLayer Exit!");
        m_NodeEditor->Save();
    }

    void OnUpdate() override{
        m_NodeEditor->Update();
    }

    void OnAsyncUpdate() override{
        m_NodeEditor->Run();
    }

    void OnRender() override{
        m_NodeEditor->Render();
    }

    void OnGUIRender() override{
        m_NodeEditor->RenderGUI();
    }

private:
    GamePlay::NodeEditor* m_NodeEditor;
    Font m_Font;
};

class Game: public Engine::Application{
public:
    Game(){
        m_EditorLayer = new EditorLayer(m_Font);
        PushLayer(m_EditorLayer);
    }
    ~Game() override {
        PopLayer(m_EditorLayer);
    }

private:
    EditorLayer* m_EditorLayer;
};

Engine::Application* Engine::Create(){
    return new Game();
}