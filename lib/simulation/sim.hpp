#pragma once

#include <thread>
#include <vector>
#include <CL/opencl.hpp>

typedef struct alignas(16) Node_connection{
    cl_int souce;
    cl_int destination;
    cl_float weight;
    cl_int rsvd;
} Node_connection;

class BurstSim{
public:
    BurstSim(int nodesInput, int nodesOutput, int nodesNormal, float density, int subticks, int networks, bool runOnCpu=false);

    //returns true if start was successfull
    bool startSimulation();

private:
    void renderSimManagerWindow();

    void simManagerTask();

public:
    static void renderSimManagerWindows();

private:
    //network parameters
    int nodesInput, nodesOutput, nodesNormal, nodesTotal;
    int connectionCount;
    int networks;
    int subticks;
    int totalTicks = 0;

    //network buffers
    //this is allocated as follows [input, normal, output]
    std::vector<float> node_buffer_1;
    std::vector<float> node_buffer_2;
    std::vector<Node_connection> connections;

    //manager
    std::thread manager;
    bool shouldRun = false;
    bool simulationReady = false;
    bool runOnCpu;

    //keeping track of all the sims
    int id; 
    static inline int idCounter = 0;   //to manage multable 
    static inline std::vector<BurstSim*> simulations;
};