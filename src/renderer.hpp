#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <scene.hpp>


class Renderer
{
public:
    Renderer();
    ~Renderer();
    Renderer(Renderer &&) = default;
    Renderer (Renderer const&) = delete;
    Renderer & operator=( Renderer &) = delete;
    Renderer & operator=( Renderer &&) = delete;

    void setWindowSize();

    void setScene( std::unique_ptr<Scene> && scene ) { M_scene = std::move(scene); }
    std::unique_ptr<Scene> & scene() { return M_scene; }; 
    void getScene();

    void run();

private:
    GLFWwindow * M_window;
    ImGuiIO & M_io; 


    ImGuiWindowFlags M_windowFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    std::unique_ptr<Scene> M_scene;


};



