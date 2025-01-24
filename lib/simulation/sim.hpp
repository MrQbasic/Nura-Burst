#include <vector>

class BurstSim{
public:
    BurstSim(int nodesInput, int nodesOutput, int nodesNormal, float density, int subticks, int networks);



private:
    int nodesInput, nodesOutput, nodesNormal, nodesTotal;
    float connections;
    int networks;
    int subticks;
    int totalTicks = 0;


    int id; 
    static inline int idCounter = 0;   //to manage multable 
    static inline std::vector<BurstSim*> simulations;
};