#include <ImGui/imgui.h>
#include <simulation/Cell.hpp>


std::vector<float> x_data;
std::vector<float> y1_data;
std::vector<float> y2_data;

float overdrive = 1.0f;
float exponent  = 3.0f;


void fill_Data(){

    Cell c(exponent, overdrive);

    const int num_points = 200;

    x_data.resize(num_points);
    y1_data.resize(num_points);
    y2_data.resize(num_points);
    
    // Linear interpolation from 1 down to -1
    for (int i = 0; i < num_points; ++i) {
        double t = static_cast<double>(i) / (num_points - 1);
        // Reverses direction: starts at 1.0, ends at -1.0
        x_data[i] = 1.0 - 2.0 * t; 
        auto[out, newinp] = c.activation_default(x_data[i]);
       
        y1_data[i] = out;
        y2_data[i] = newinp;
    }

}


void renderWindow_Cell(){
    ImGui::Begin("Cell", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        fill_Data();

        ImGui::InputFloat("overdrive", &overdrive, 0.1f);
        ImGui::InputFloat("exponent", &exponent, 0.1f);
    

        if(ImPlot::BeginPlot("activation", ImVec2(0,0))){
            ImPlot::SetupAxesLimits(ImAxis_X1, -1.1, 1.1, ImGuiCond_Always);
            ImPlot::PlotLine("output", x_data.data(), y1_data.data(), x_data.size());
            ImPlot::PlotLine("input", x_data.data(), x_data.data(), x_data.size());
            ImPlot::PlotLine("left", x_data.data(), y2_data.data(), x_data.size());
            ImPlot::EndPlot();
        }

    ImGui::End();
}