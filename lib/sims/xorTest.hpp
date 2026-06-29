#include <simulation/sim.hpp>
#include <random>

class XorTest : public BurstSim{
public:
    void injectNetwork(int netID) override{
        return;

        //this fucks performance
        if(!init){
            //check the input count
            if(this->nodesInput < 2){
                std::cout << "You Idiot you need 2 inputs for an XOR" << std::endl;
                this->shouldRun = false;
                return;
            }
            init = true;
        }
        //push 2 random values
        this->node_buffer_1[netID * nodesTotal + 0] = distrib(gen);
        this->node_buffer_1[netID * nodesTotal + 1] = distrib(gen);
    }

    float extractNetwork(int netID) override{
        bool inp1 = node_buffer_1[netID * nodesTotal + 0];
        bool inp2 = node_buffer_1[netID * nodesTotal + 1];

        bool res = inp1 ^ inp2;
        
        float expected = 0.0f;
        if(res){
            expected = 1.0f;
        }

        float out = node_buffer_1[netID * nodesTotal + (nodesInput+nodesNormal)];

        return fabs(out-expected);
    }

    using BurstSim::BurstSim;

private:
    std::random_device rd;
    std::mt19937 gen{rd()}; 
    std::uniform_int_distribution<int> distrib{0, 1};

    bool init = false;

};
