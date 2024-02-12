#include <GameEngine/Core/Application.h>
#include <GameEngine/Core/EntryPoint.h>
#include "EditorLayer.h"

namespace RendererEngine{
    class EngineEditor : public Application{
    public:
        EngineEditor(ApplicationCommandLineArgs args) : Application("Engine Editor", args) {
            pushLayer(new EditorLayer());
        }

        ~EngineEditor() {}
    };

    Application* CreateApplication(ApplicationCommandLineArgs args){
		return new EngineEditor(args);
	}
};