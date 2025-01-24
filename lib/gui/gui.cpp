#include <gui/gui.hpp>
#include <gui/menu/menu.hpp>

//env vars
GLFWwindow* window;

bool setupGui(){
    //init glfw
    if(!glfwInit()){
        std::cout << "Error: Couldn't init GLFW!" << std::endl;
        return false;
    }
    //create window
    window = glfwCreateWindow(720, 480, "Burst", 0, 0);
    if(!window){
        std::cout << "Error: Couldn't create Window" << std::endl;
        return false;
    }
    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif
    
    glfwMakeContextCurrent(window);
    //imgui contxt
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    //renderer
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImPlot::CreateContext();

    ImGui::StyleColorsDark();

    return true;
}



bool renderGui(){
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0){
        ImGui_ImplGlfw_Sleep(10);
        return !glfwWindowShouldClose(window);
    }
        
    //prepare new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //rendering of the menu
    renderMenu();




    //Rendering of the current frame
    ImGui::Render();
    int sw, sh;
    glfwGetFramebufferSize(window, &sw, &sh);
    glViewport(0,0, sw, sh);
    glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    //check if the window should close
    return !glfwWindowShouldClose(window);
}
