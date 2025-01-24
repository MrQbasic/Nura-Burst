//#include <stdio.h>
//#include <math.h>

//#include "../lib/csv.h"
//#include "../lib/Network.h"

//#define networks 100

#include<gui/gui.hpp>

int main() {
    setupGui();

    while(renderGui());
}



/*
int main(){

    struct NETWORK* net[networks];
    for(int i=0; i<networks; i++){
        net[i] = network_Setup(100, 10, 3, 3);
    }


    float total_best = 1000.0f;
    for(int i=0; i<10000; i++){
        float best_score = 0.0f;
        float avg_score = 0.0f;
        int best_index = -1;
        int failcnt = 0;
        for(int j=0; j<networks; j++){

            //execute the network

            float input = (float) rand() / (float) RAND_MAX;

            for(int l=0; l < net[j]->inj_Count; l++){
                *(net[j]->inj_s[l]) = input;
            }
            float batchEnergy = network_run(net[j],1000);
            float score = fabs(sinf(input) - net[j]->ext_s[0]);


            //make shure we dont have any braindead ones
            if(batchEnergy <= 0.1f){ //I hate floats
                score = -1.0f;
                failcnt ++;
            }else{
                avg_score += score;
            }


            //printf("batch energy: %f error: %f \n", batchEnergy, score);
            

            //check for a new best
            if(score < best_score && score >= 0 || best_index == -1 && score >= 0){
                best_score = score;
                best_index = j;
            }
            if(score < total_best && score >= 0 ){
                total_best = score;
            }

        }
        avg_score /= networks - failcnt;

        if(best_index == -1) {
            best_index = 0;
            printf("ALL NETWORKS FAILED!!\n");
        }


        for(int l=0; l<networks; l++){
            if(l == best_index) continue;
            network_mutate(net[best_index],net[l],0.0005f);
        }


        printf("Best: %f AVG: %f Total Best: %f Best: %d Fucked: %d\n",best_score, avg_score, total_best, best_index, failcnt);
        //printf("%f %f %f\n",best_score, avg_score, total_best);
        cvs_add_line(best_score, avg_score, total_best);
        //printf("%f %f\n",best_score, avg_score);
        //printf("%f\n",best_score);
    }

    return 0;
}

*/