#pragma once

#include <thread>
#include <vector>
#include <CL/opencl.hpp>
#include <iostream>
#include <simulation/Cell.hpp>

//data per Node
typedef struct alignas(16) Node_connection{
    cl_int souce;
    cl_int destination;
    cl_float weight;
    cl_int rsvd;
} Node_connection;

//used for analysis
typedef struct Stats_perTick{
    float error_max, error_min, error_avg;
} Stats_perTick;

class BurstSim{
public:
    BurstSim(int nodesInput, int nodesOutput, int nodesNormal, float density, int subticks, int networks, bool runOnCpu=false);

    //returns true if start was successfull
    bool startSimulation();

    virtual void injectNetwork(int netID)  { std::cout << "Don't use the base class you idiot!\n";}

    //returns an error which is used for logging and training
    virtual float extractNetwork(int netID)  { std::cout << "Don't use the base class you idiot!\n"; return 0.0;}

private:
    void renderSimManagerWindow();

    void simManagerTask();

public:
    static void renderSimManagerWindows();

protected:
    //network parameters
    int nodesInput, nodesOutput, nodesNormal, nodesTotal;
    int connectionCount;
    int networks;
    int subticks;
    int totalTicks = 0;

    //all the nodes
    std::vector<Cell> cells;

    //statistics
    std::vector<Stats_perTick> statistics;

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