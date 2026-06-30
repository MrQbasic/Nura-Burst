#pragma once

#include <vector>
#include <random>
#include <simulation/Cell.hpp>


struct Network_Outline {
    //cells
    int node_count_Input;
    int node_count_Output;
    int node_count_Normal;
    //cell - settings
    float exponent;
    float overdrive;
    //connections
    float connection_Density;
    //simulations
    int subticks;
};

class Network {
public:
    Network(const Network_Outline& outline)
        : nodesInput(outline.node_count_Input),
          nodesOutput(outline.node_count_Output),
          nodesNormal(outline.node_count_Normal),
          nodesTotal(outline.node_count_Input + outline.node_count_Normal + outline.node_count_Output),
          subticks(outline.subticks),
          overdrive(outline.overdrive),
          exponent(outline.exponent),
          connectionCount(static_cast<int>(nodesTotal * outline.connection_Density))
    {
        allocateCells();
        connectCells_init();
    }

    //tick the networks once
    float* burst(float* input){
        return nullptr;
    }



private:
    void allocateCells() {
        cells.reserve(nodesTotal);
        for (int i = 0; i < nodesTotal; ++i) {
            bool isDummy = (i < nodesInput) || (i >= nodesInput + nodesNormal);
            cells.emplace_back(exponent, overdrive, isDummy);
        }
    }

    void connectCells_init() {
        if (nodesTotal < 2) {
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rndIndex(0, nodesTotal - 1);
        std::uniform_real_distribution<float> rndWeight(0.0f, 1.0f);

        for (int i = 0; i < connectionCount; ++i) {
            int destination = rndIndex(gen);
            int source = rndIndex(gen);
            cells[destination].connectInput(&cells[source], rndWeight(gen));
        }
    }

public:
    //Network dimensions
    int nodesInput;
    int nodesOutput;
    int nodesNormal;
    int nodesTotal;
    int connectionCount;
    int subticks;
    float overdrive;
    float exponent;
    std::vector<Cell> cells;
};
