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

BurstSim::BurstSim(const Network_Outline& outline, int networks, bool runOnCpu)
{
    this->outline = outline;
    this->nodesInput = outline.node_count_Input;
    this->nodesNormal = outline.node_count_Normal;
    this->nodesOutput = outline.node_count_Output;
    this->nodesTotal = outline.node_count_Input + outline.node_count_Normal + outline.node_count_Output;
    this->connectionCount = static_cast<int>(nodesTotal * outline.connection_Density);
    this->networkCount = networks;
    this->id = idCounter++;
    this->runOnCpu = runOnCpu;
    this->subticks = std::max(outline.subticks, 1);

    networkInstances.reserve(networkCount);
    for (int i = 0; i < networkCount; ++i) {
        networkInstances.emplace_back(outline);
    }

    // save to simBuffer for keeping track of all sims
    simulations.push_back(this);
    // start up the manager task for preparation
    this->manager = std::thread(&BurstSim::simManagerTask, this);
}

bool BurstSim::startSimulation()
{
    if (this->simulationReady) {
        this->shouldRun = true;
        return true;
    }
    return false;
}

std::string loadKernel(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void BurstSim::simManagerTask()
{
    if (networkInstances.empty()) {
        this->simulationReady = true;
        return;
    }

    std::cout << "Networks initialized!" << std::endl;

    // wait for the go
    this->simulationReady = true;
    while (!shouldRun) {
        std::this_thread::yield();
    }
    std::cout << "Sim started!" << std::endl;

    // do until the sim is killed
    while (shouldRun) {
        std::this_thread::yield();
    }
}

void BurstSim::renderSimManagerWindow(){
    bool die = false;

    std::string title = std::format("Simulation  ID: {}##xx", this->id);
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
