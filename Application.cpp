#include <Engine3D/Core/Application.h>
#include <Engine3D/Core/EntryPoint.h>
#include "EditorLayer.h"

namespace Engine3D{
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