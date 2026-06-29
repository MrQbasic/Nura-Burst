#pragma once
#include <ImGui/imgui.h>

#include <simulation/sim.hpp>
#include <gui/windows/trainSettings.hpp>
#include <gui/windows/Cell-window.hpp>

bool menuOpen_SimSettings = true;
bool menuOpen_Cell = true;



void renderMenu(){
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("View ")){
        ImGui::MenuItem("Train Settings", nullptr, &menuOpen_SimSettings);

        ImGui::MenuItem("Cell Debug", nullptr, &menuOpen_Cell);
        
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    
    
    if(menuOpen_SimSettings){
        renderWindow_TrainSettings();

        BurstSim::renderSimManagerWindows();
    }

    if(menuOpen_Cell){
        renderWindow_Cell();

        BurstSim::renderSimManagerWindows();
    }
}