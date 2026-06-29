#include <simulation/sim.hpp>
#include <ImGui/imgui.h>
#include <ImGui/implot.h>
#include <iostream>
#include <algorithm>
#include <CL/opencl.hpp>
#include <fstream>
#include <sstream>
#include <random>
#include <syncstream>
#include <chrono>
#include <format>

BurstSim::BurstSim(int nodesInput, int nodesOutput, int nodesNormal, float density, int subticks, int networks, bool runOnCpu){
    //getting the general dimensions of the network
    this->nodesInput = nodesInput;
    this->nodesNormal = nodesNormal;
    this->nodesOutput = nodesOutput;
    this->nodesTotal = nodesInput + nodesNormal + nodesOutput;
    this->connectionCount = (int)((float)nodesTotal * density);
	this->networks = networks;
    this->id = idCounter; idCounter++;
    this->runOnCpu = runOnCpu;
    if(subticks < 1){
        this->subticks = 1;
    }else{
        this->subticks = subticks;
    }
    //save to simBuffer for keeping track of all sims
    simulations.push_back(this);
    //start up the manager task for preperation
    this->manager = std::thread(&BurstSim::simManagerTask, this);

}

bool BurstSim::startSimulation(){
    if(this->simulationReady){
        this->shouldRun = true;
        return true;
    }else{
        return false;
    }
}

std::string loadKernel(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void BurstSim::simManagerTask(){
    //allocate all the Cells
    for(int i=0; i<this->nodesTotal; i++){
        Cell c;
        this->cells.push_back(c);
    }

    //connect the cells
    for(int i=0; i<conn; i++)

}

void BurstSim::renderSimManagerWindow(){
    bool die = false;

    std::string title = std::format("Simulation  ID: {}", this->id);
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if(ImGui::BeginTabBar("MainTabBar")){
            if(ImGui::BeginTabItem("General")){
                ImGui::Text("Total Ticks: %d", totalTicks);
                //Status Control
                ImGui::Text("Status: %s", (this->simulationReady ? (this->shouldRun ? ("Running") : ("Ready")) : ("Initializing..")));
                ImGui::BeginDisabled(shouldRun || !simulationReady);
                if(ImGui::Button("Start")) this->startSimulation();
                ImGui::EndDisabled();
                ImGui::BeginDisabled(!shouldRun);
                die = ImGui::Button("Kill");
                ImGui::EndDisabled();
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Training")){
                if(ImPlot::BeginPlot("Error")){
                    int count = static_cast<int>(this->statistics.size());

                    ImPlotSpec spec;
                    spec.Stride = sizeof(Stats_perTick);

                    ImPlot::PlotLine("MAX", &this->statistics[0].error_max, count, 1, 0, spec);
                    ImPlot::PlotLine("MIN", &this->statistics[0].error_min, count, 1, 0, spec);
                    ImPlot::PlotLine("AVG", &this->statistics[0].error_avg, count, 1, 0, spec);
                    
                    ImPlot::EndPlot();
                }


                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Training-Settings")){

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    ImGui::End();

    if(die){ 
        //handle the manager
        this->shouldRun = false;
        this->manager.join();
        simulations.erase(
            std::remove(simulations.begin(), simulations.end(), this),
            simulations.end()
        );
        delete this;
    }
}

//static wrapper
void BurstSim::renderSimManagerWindows(){
    for(auto sim : simulations){
        sim->renderSimManagerWindow();
    }
}
