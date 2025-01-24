#include <ImGui/imgui.h>
#include <simulation/sim.hpp>

#define sim_minNodeCount 1
int sim_nodeCount_input  = 1;
int sim_nodeCount_output = 1;
int sim_nodeCount_normal = 1;

int sim_subticks = 0;
float sim_connDensity = 0.5f;
int sim_networkCount = 1000;

bool sim_isRunning = false;

void renderWindow_TrainSettings(){
    ImGui::Begin("Training");
        //Set dimensions of the network
        ImGui::Text("Dimension:");
            ImGui::InputInt("Input  Nodes", &sim_nodeCount_input);
            ImGui::InputInt("Output Nodes", &sim_nodeCount_output);
            ImGui::InputInt("Normal Nodes", &sim_nodeCount_normal);
            sim_nodeCount_input = sim_nodeCount_input   < sim_minNodeCount ? sim_minNodeCount : sim_nodeCount_input;
            sim_nodeCount_output = sim_nodeCount_output < sim_minNodeCount ? sim_minNodeCount : sim_nodeCount_output;
            sim_nodeCount_normal = sim_nodeCount_normal < sim_minNodeCount ? sim_minNodeCount : sim_nodeCount_normal;
        ImGui::Text("Mutation:");
            ImGui::InputFloat("Connections Density", &sim_connDensity);
            //subtick time is time networks ticks between injections/extractions
        ImGui::Text("Simulation:");
            ImGui::InputInt("Subticks", &sim_subticks);
            sim_subticks = sim_subticks < 0 ? 0 : sim_subticks;
            ImGui::InputInt("Network Count", &sim_networkCount);
            
            ImGui::BeginDisabled(sim_isRunning);
            if(ImGui::Button("Start")){
                //call start
                BurstSim* sim = new BurstSim(sim_nodeCount_input, sim_nodeCount_output, sim_nodeCount_normal, sim_connDensity, sim_subticks, sim_networkCount);
                sim_isRunning=true;
            }
            ImGui::EndDisabled();
            ImGui::BeginDisabled(!sim_isRunning);
            if(ImGui::Button("Stop")){
                //call start
                sim_isRunning=false;
            }
            ImGui::EndDisabled();


    ImGui::End();
}