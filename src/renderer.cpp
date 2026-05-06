#include <renderer.hpp>


Renderer::Renderer()
    : M_io(ImGui::GetIO())
{


    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


    M_window = glfwCreateWindow(1280, 720, "Physics Engine", nullptr, nullptr);

    glfwMakeContextCurrent(M_window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(M_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}



Renderer::~Renderer()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(M_window);
    glfwTerminate();
};




void
Renderer::setWindowSize()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
}




void
Renderer::getScene()
{
    ImGui::Begin("Scene",nullptr, M_windowFlags);

    Inspector::instance()->render();


    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    M_scene->render( drawList );

    ImGui::End();
}



void
Renderer::run()
{
    while (!glfwWindowShouldClose(M_window))
    {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        setWindowSize();


        getScene();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(M_window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(M_window);
    }
}



