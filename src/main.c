#include <stdio.h>
#include <math.h>

#include "../lib/csv.h"
#include "../lib/Network.h"

#define networks 100

int main(){

    struct NETWORK* net[networks];
    for(int i=0; i<networks; i++){
        net[i] = network_Setup(10, 3, 3, 2);
    }


    float total_best = 1000.0f;
    for(int i=0; i<1000; i++){
        float best_score = 1000.0f;
        float avg_score = 0.0f;
        int best_index;
        for(int j=0; j<networks; j++){
            for(int l=0; l<3; l++){
                *(net[j]->inj_s[l]) = 3.0f;
            }
            float score = fabs(2.0f - sin(fabs(network_run(net[j],100))));

            if(avg_score == 0.0f){
                avg_score = score;
            }else{
                avg_score = (avg_score + score) / 2;
            }

            if(score < best_score){
                best_score = score;
                best_index = j;
            }

            if(score < total_best){
                total_best = score;
            }

        }
        for(int l=0; l<networks; l++){
            network_mutate(net[best_index],net[l],0.05f);
        }
        //printf("%f %f %f %d\n",best_score, avg_score, total_best, best_index);
        printf("%f %f %f\n",best_score, avg_score, total_best);
        cvs_add_line(best_score, avg_score, total_best);
        //printf("%f %f\n",best_score, avg_score);
        //printf("%f\n",best_score);
    }

    return 0;
}