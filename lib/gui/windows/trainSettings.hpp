#include <vector>
#include <ImGui/imgui.h>
#include <ImGui/implot.h>
#include <simulation/sim.hpp>

#define sim_minNodeCount 1
Network_Outline sim_network_outline = {
    .node_count_Input = 1,
    .node_count_Output = 1,
    .node_count_Normal = 1,
    .exponent = 3.0f,
    .overdrive = 2.0f,
    .connection_Density = 0.5f,
    .subticks = 0,
};

int sim_networkCount = 1000;
bool cpuExecution = false;

void renderWindow_TrainSettings(){
    ImGui::Begin("New Simulations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        //Set dimensions of the network
        ImGui::Text("Dimension:");
            ImGui::InputInt("Input  Nodes", &sim_network_outline.node_count_Input);
            ImGui::InputInt("Output Nodes", &sim_network_outline.node_count_Output);
            ImGui::InputInt("Normal Nodes", &sim_network_outline.node_count_Normal);
            sim_network_outline.node_count_Input = sim_network_outline.node_count_Input < sim_minNodeCount ? sim_minNodeCount : sim_network_outline.node_count_Input;
            sim_network_outline.node_count_Output = sim_network_outline.node_count_Output < sim_minNodeCount ? sim_minNodeCount : sim_network_outline.node_count_Output;
            sim_network_outline.node_count_Normal = sim_network_outline.node_count_Normal < sim_minNodeCount ? sim_minNodeCount : sim_network_outline.node_count_Normal;
        ImGui::Text("Mutation:");
            ImGui::InputFloat("Connections Density", &sim_network_outline.connection_Density);
            sim_network_outline.connection_Density = sim_network_outline.connection_Density < 0.0f ? 0.0f : sim_network_outline.connection_Density;
            sim_network_outline.connection_Density = sim_network_outline.connection_Density > 1.0f ? 1.0f : sim_network_outline.connection_Density;
        ImGui::Text("Simulation:");
            ImGui::InputInt("Subticks", &sim_network_outline.subticks);
            sim_network_outline.subticks = sim_network_outline.subticks < 1 ? 1 : sim_network_outline.subticks;
            ImGui::InputInt("Network Count", &sim_networkCount);

        ImGui::Checkbox("Run on CPU", &cpuExecution);


        //Cell Settings
        if (ImGui::CollapsingHeader("Cell Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            //handle the inputs
            ImGui::InputFloat("Overdrive", &sim_network_outline.overdrive, 0.1f);
            ImGui::InputFloat("Exponent", &sim_network_outline.exponent, 0.1f);
            sim_network_outline.overdrive = sim_network_outline.overdrive < 0.0f ? 0.0f : sim_network_outline.overdrive;
            sim_network_outline.exponent = sim_network_outline.exponent < 0.0f ? 0.0f : sim_network_outline.exponent;
            //define data area
            const int num_points = 200;
            static std::vector<float> x_data;
            static std::vector<float> y_out;
            static std::vector<float> y_left;
            x_data.resize(num_points);
            y_out.resize(num_points);
            y_left.resize(num_points);
            //collect data points
            Cell c(sim_network_outline.exponent, sim_network_outline.overdrive);
            for (int i = 0; i < num_points; ++i) {
                float t = static_cast<float>(i) / (num_points - 1);
                x_data[i] = 1.0f - 2.0f * t;
                auto [out, left] = c.activation_default(x_data[i]);
                y_out[i] = out;
                y_left[i] = left;
            }
            //plot the data
            if (ImPlot::BeginPlot("Activation", ImVec2(0, 0), ImPlotFlags_NoInputs | ImPlotFlags_NoMouseText | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect)) {
                ImPlot::SetupAxesLimits(-1.0f, 1.0f, -1.0f, 1.0f);
                ImPlot::PlotLine("output", x_data.data(), y_out.data(), static_cast<int>(x_data.size()));
                ImPlot::PlotLine("left", x_data.data(), y_left.data(), static_cast<int>(x_data.size()));
                ImPlot::PlotLine("input", x_data.data(), x_data.data(), x_data.size());
                ImPlot::EndPlot();
            }
        }

        if(ImGui::Button("Add")){
            //call start
            BurstSim* sim = new BurstSim(sim_network_outline, sim_networkCount, cpuExecution);
        }

    ImGui::End();
}