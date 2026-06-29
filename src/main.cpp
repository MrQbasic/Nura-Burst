//#include <stdio.h>
//#include <math.h>

//#include "../lib/csv.h"
//#include "../lib/Network.h"

//#define networks 100
#include <iostream>
#include <gui/gui.hpp>
#include <string>
#include <simulation/sim.hpp>

int main(int argc, char *argv[]) {
    bool nogui = false;
    
    //parse args
    for(int i=1; i<argc; i++){
        std::string arg = argv[i];

        if(arg == "--nogui"){
           nogui = true;
        }else{
            std::cout << "Unknown command line option:" << argv[i] << std::endl;
            return 0;
        }
    }

    if(nogui){
        BurstSim sim(1, 1, 1000, 0.5f, 1000, 1000, true);
        while(1){
            
        }

    }else{
        setupGui();
        while(renderGui());
    }
}