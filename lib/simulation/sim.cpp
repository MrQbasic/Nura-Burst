#include <simulation/sim.hpp>
#include <ImGui/imgui.h>
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
    std::osyncstream(std::cout) << "Sim ID: " << this->id << " -> " << "created" << std::endl; 
    
    //openCL device setup
    cl::Device device = cl::Device::getDefault();
    cl::Context context(device);
    cl::CommandQueue queue(context, device);
    std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "OpenCL device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    
    //get the programs
    cl::Program programm_activation(context, loadKernel("../lib/kernels/activation.cl"));
    cl::Program programm_burst(context, loadKernel("../lib/kernels/burst.cl"));
    if(programm_activation.build(device) != CL_SUCCESS){
        std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Build error:\n" << programm_activation.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return;
    }
    if(programm_burst.build(device) != CL_SUCCESS){
        std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Build error:\n" << programm_burst.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return;
    }
    cl::Kernel kernel_activation(programm_activation, "activaion");
    cl::Kernel kernel_burst(programm_burst, "burst");
    
    //node buffers
    try{
        node_buffer_1.assign(networks * nodesTotal, {0});
        node_buffer_2.assign(networks * nodesTotal, {0});
    }catch (const std::bad_alloc& e){
        std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Node vector alloc failed: " << e.what() << std::endl;
        return;
    }
    cl_int err;
    cl::Buffer buf_nodes_1  (context, CL_MEM_READ_WRITE  | CL_MEM_COPY_HOST_PTR, sizeof(float) * nodesTotal * networks, node_buffer_1.data(), &err);
    if(err != CL_SUCCESS){
        std::osyncstream(std::cerr)<< "Sim ID: " << this->id << " -> " << "Node buffer 1 alloc GPU failed: " << err << std::endl;
        return;
    }
    cl::Buffer buf_nodes_2  (context, CL_MEM_READ_WRITE  | CL_MEM_COPY_HOST_PTR, sizeof(float) * nodesTotal * networks, node_buffer_2.data(), &err);
    if(err != CL_SUCCESS){
        std::osyncstream(std::cerr)<< "Sim ID: " << this->id << " -> " << "Node buffer 2 alloc GPU failed: " << err << std::endl;
        return;
    }

    //connection buffer
    int originCount = nodesNormal + nodesInput;
    int destinationCount = nodesNormal + nodesOutput;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr_origin(0, originCount);
    std::uniform_int_distribution<> distr_destination(0, destinationCount);
    std::uniform_real_distribution<float> distr_weight(-1.0f, 1.0f);
    std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Connection count: " << connectionCount << std::endl; 
    std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Network count: " << networks << std::endl;
    std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Origin Node count: " << originCount << std::endl;
    std::osyncstream(std::cout)<< "Sim ID: " << this->id << " -> " << "Destination Node count: " << destinationCount << std::endl;
    //try allocating the size of the connection buffer
    try{
        this->connections.reserve(connectionCount * networks);
    }catch (const std::bad_alloc& e){
        std::osyncstream(std::cout) << "Sim ID: " << this->id << " -> " << "Connection vector alloc failed: " << e.what() << std::endl; 
        return;
    }
    for(int i=0; i<this->connectionCount * networks; i++){
        Node_connection conn;
        conn.souce = distr_origin(gen);
        conn.destination = distr_destination(gen) + nodesInput;
        conn.weight = distr_weight(gen);
        this->connections.push_back(conn);
    }
    //pushing the buffer to the GPU
    cl::Buffer buf_connections (context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * connectionCount * networks, connections.data(), &err);
    if(err != CL_SUCCESS){
        std::osyncstream(std::cerr)<< "Sim ID: " << this->id << " -> " << "Connection buffer alloc GPU failed: " << err << std::endl;
        return;
    }

    //call sizes are base on the network dimensions
    cl::NDRange size_perNode(nodesTotal, networks);
    cl::NDRange size_perConnection(connectionCount, networks);
    
    //add the paramerts for activation function
    kernel_activation.setArg(0, buf_nodes_1);
    kernel_activation.setArg(1, buf_nodes_2);
    kernel_activation.setArg(2, nodesTotal);
    kernel_activation.setArg(3, nodesOutput);


    kernel_burst.setArg(0, buf_nodes_1);
    kernel_burst.setArg(1, buf_nodes_2);
    kernel_burst.setArg(2, buf_connections);
    kernel_burst.setArg(3, connectionCount);  //connections pernetwork
    kernel_burst.setArg(4, nodesTotal);       //nodes per netwrok
    std::osyncstream(std::cout) << "Sim ID: " << this->id << " -> " << "Init done" << std::endl;

    //UNTIL HERE IS PREPARE! THIS WAITS FOR THE START command
    this->simulationReady = true;
    while(!shouldRun){
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    if(this->runOnCpu){
        std::osyncstream(std::cerr)<< "Sim ID: " << this->id << " -> " << "Started on CPU" << std::endl;
        while(shouldRun){
            for(int i=0; i<subticks; i++){
                //run per networks
                for(int n=0; n<networks; n++){
                    //activation function
                    float exponent = 15.0f; // exponent for the activation function
                    for(int node=0; node<(nodesInput+nodesNormal); node++){
                        float input = node_buffer_1[n*nodesTotal + node];
                        float s1 = 1.0f / (1.0f + exp((-exponent)* (input-0.5f)));
                        float s2 = 1.0f / (1.0f + exp(  exponent * (input+0.5f)));
                        float activationOut = s1 + s2;
                        node_buffer_2[n*nodesTotal + node] = activationOut;
                        node_buffer_2[n*nodesTotal + node] -= fabs(activationOut) * input;
                    }
                    //burst function
                    for(int con=0; con<connectionCount; con++){
                        Node_connection* connection = &(connections[n*connectionCount + con]);
                        float input = node_buffer_2[n*nodesTotal+connection->souce];
                        node_buffer_1[n*nodesTotal+connection->destination] += input * connection->weight;
                    }
                }
            }
            this->totalTicks++;
        }
    }else{
        std::osyncstream(std::cerr)<< "Sim ID: " << this->id << " -> " << "Started on GPU" << std::endl;
        while(shouldRun){
            for(int i=0; i<subticks ; i++){
                //push per node (activation function, reset of output nodes)
                queue.enqueueNDRangeKernel(
                    kernel_activation,
                    cl::NullRange,
                    size_perNode,
                    cl::NullRange
                );
                queue.enqueueNDRangeKernel(
                    kernel_burst,
                    cl::NullRange,
                    size_perConnection,
                    cl::NullRange
                );
            }
            //wait for the GPU to finish
            queue.finish();

            //evaluate the networks


            this->totalTicks++;
        }
    }
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
