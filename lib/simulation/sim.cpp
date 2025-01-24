#include <simulation/sim.hpp>
#include <iostream>


BurstSim::BurstSim(int nodesInput, int nodesOutput, int nodesNormal, float density, int subticks, int networks){
    //getting the general dimensions of the network
    this->nodesInput = nodesInput;
    this->nodesNormal = nodesNormal;
    this->nodesOutput = nodesOutput;
    this->nodesTotal = nodesInput + nodesNormal + nodesOutput;
    this->connections = (int)((float)nodesTotal / density);
    this->id = idCounter; idCounter++;
    this->subticks = subticks;
    //save to simBuffer for keeping track of all sims
    simulations.push_back(this);
    //setup buffers the networks live in
    

    //
    std::cout << "New simulation created with ID: " << id << std::endl;
}