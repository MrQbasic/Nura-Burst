#pragma once
#include <ImGui/imgui.h>

#include <simulation/sim.hpp>
#include <gui/windows/trainSettings.hpp>


bool menuOpen_SimSettings = true;



void renderMenu(){
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("View ")){
        ImGui::MenuItem("Train Settings", nullptr, &menuOpen_SimSettings);

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    
    
    if(menuOpen_SimSettings){
        renderWindow_TrainSettings();

        BurstSim::renderSimManagerWindows();
    }
}