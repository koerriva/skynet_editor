#include <brain/NodeEditor.h>
#include "engine/Engine.h"

class EditorLayer: public Engine::Layer{
public:
    EditorLayer(Camera2D camera,Font font){
        m_NodeEditor = new GamePlay::NodeEditor();
        m_Camera = camera;
        m_Font = font;
    }

    void OnAttach() override{
        m_NodeEditor->Load(m_Camera,m_Font);
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
        m_NodeEditor->Show();
    }

    void OnGUIRender() override{
        m_NodeEditor->ShowMenu();
    }

private:
    GamePlay::NodeEditor* m_NodeEditor;
    Camera2D m_Camera;
    Font m_Font;
};

class Game: public Engine::Application{
public:
    Game(){
        LoadFont();
        m_EditorLayer = new EditorLayer(m_Camera2D,m_Font);
        PushLayer(m_EditorLayer);
    }
    ~Game() override{
        PopLayer(m_EditorLayer);
    }

private:
    void LoadFont(){
        //基本汉字 4E00-9FA5
        //兼容汉字 F900-FAD9
        //基本字符 0-ff
        std::vector<int> fontChars;

        TraceLog(LOG_INFO,"Init SC Font Memory");
        for (int i = 0; i <= 0xFF; ++i) {
            fontChars.push_back(i);
        }
        fontChars.push_back(0x3000);
        for (int i = 0xff01; i <= 0xff0f; ++i) {
            fontChars.push_back(i);
        }
        for (int i = 0x4E00; i <= 0x9FA5; ++i) {
            fontChars.push_back(i);
        }

        TraceLog(LOG_INFO,"Load SC Font");
//        m_Font = LoadFontEx("data/font/NotoSansSC-Regular.otf",16,fontChars.data(),fontChars.size());
        m_Font = LoadFontEx("data/font/LiHeiPro.ttf",16,fontChars.data(),fontChars.size());
        TraceLog(LOG_INFO,"SC Font Load");
        fontChars.clear();
    }

private:
    Font m_Font;
    EditorLayer* m_EditorLayer;
};

Engine::Application* Engine::Create(){
    return new Game();
}