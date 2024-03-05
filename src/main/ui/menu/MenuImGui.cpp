#include "MenuImGui.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <math.h>

#include "Settings.h"
#include "ui/Gui.h"
#include "ui/Input.h"

#include "graphics/Graphics.h"
#include "logger/logger.h"

#include "time/SystemTime.h"
#include "io/MemoryManager.h"
#include "graphics/Image.h"
#include "graphics/Texture.h"
#include "graphics/TextureOpenGl.h"
#include "script/ScriptEngine.h"
#include "script/Script.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

Menu& Menu::getInstance() {
    static MenuImGui menu = MenuImGui();
    return menu;
}

unsigned int MenuImGui::getWidth() {
    return WindowSdl::getWidth();
}

unsigned int MenuImGui::getHeight() {
    return WindowSdl::getHeight();
}

bool MenuImGui::init() {
    loggerTrace("Init menu");

    WindowSdl::init();

    setDimensions(800, 400);
    setTitle(Settings::demo.title);

    return true;  // FIXME
}
bool MenuImGui::exit() {
    loggerTrace("Exit menu");
    Graphics::getInstance().exit();
    return WindowSdl::exit();
}
bool MenuImGui::open() {
    loggerDebug("Open menu");

    if (!WindowSdl::open()) {
        return false;
    }

#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glslVersion = "#version 150";
#else
    // GL 3.0 + GLSL 130
    const char* glslVersion = "#version 130";
#endif

    // Setup ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL2_InitForOpenGL(window, static_cast<SDL_GLContext>(getGraphicsContext()));
    ImGui_ImplOpenGL3_Init(glslVersion);

    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_RETURN2;
    io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    //MemoryManager<Script>& scriptMemory = MemoryManager<Script>::getInstance();
    //Script *script = scriptMemory.getResource(std::string("_embedded/Menu.js"), true);
    //script->load();

    ScriptEngine::getInstance().evalString("var menu = new Menu();");

    // Main loop
    setQuit(true);
    Input& input = Input::getInstance();
    while (!input.isUserExit()) {
        input.pollEvents();

        Graphics& graphics = Graphics::getInstance();
        graphics.setViewport(0, 0, getWidth(), getHeight());
        graphics.setClearColor(Color(0, 0, 0, 0));
        graphics.clear();

        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        swapBuffers();
        SystemTime::sleepInMillis(10);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}

void MenuImGui::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    ScriptEngine::getInstance().evalString("menu.render();");
}

bool MenuImGui::close() {
    loggerDebug("Close menu");
    return WindowSdl::close();
}
