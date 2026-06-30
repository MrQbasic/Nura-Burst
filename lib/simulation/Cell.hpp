#pragma once

#include <tuple>
#include <cmath>

class Cell{
public:
    Cell(float exponent = 3.0f, float overdrive = 2.0f, bool isDummyCell=false){
        this->dummyCell = isDummyCell;
        this->input_buffer  = 0.0f;
        this->output_buffer = 0.0f;
        this->overdrive = overdrive;
        this->exponent = exponent;
    }

public:
    //output state after last burst
    float getOutput(){
        return this->output_buffer;
    }

    //call this to make the cell register its input from the last burst
    void tickCell(){
        if(dummyCell) return; //don't do the activation function if it's a dummy
        auto[out, inp] = activation_default(input_buffer);
        this->output_buffer = out;
        this->input_buffer = inp;
    }

    //burst the cell
    void burst(){
        this->input_buffer = 0.0f;
        for(const auto& con : connection_inputs) {
            this->input_buffer += con->getOutput();
        }
    }

    void connectInput(Cell* source, float weight){
        this->connection_inputs.push_back(source);
        this->weights.push_back(weight);
    }


public:
    //output, left over in the input
    std::tuple<float, float> activation_default(float input){
        float activ =  pow(input, this->exponent);
        float newinp = input - activ;
        float output = 2.0f / (1.0f + exp(- activ * overdrive * 3.5));
        output -= 1.0f;
        return {output, newinp};
    }


protected:
    //input and output bufferting to make a burst possible
    float input_buffer;
    float output_buffer;

    //activation function
    float exponent;
    float overdrive;

    bool dummyCell = false;

    //connection between cells
    std::vector<Cell*> connection_inputs;
    std::vector<float> weights;

};