#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct NODE{
    float accum;

    float output_activation;
    float output_charge;

    int     con_count;
    float** con_prt;
    float*  con_weight;
};


struct NETWORK{
    //Nodes
    int          node_Count;
    struct NODE* node_s;
    //Injector
    int     inj_Count;
    float** inj_s;
    //Extractor
    int     ext_Count;
    float*  ext_s;
};



struct NETWORK* network_Setup(int nodes, int io_ratio, int injectors, int extractors){
    //alloc all network struct
    struct NETWORK* net = malloc(sizeof(struct NETWORK));
    //alloc nodes
    net->node_Count = nodes;
    net->node_s     = malloc(sizeof(struct NODE) * nodes);
    if(net == 0) return 0;
    //initalize nodes with random parameters
    //--setup random activation + output power
    for(int i=0; i<nodes; i++){
        net->node_s[i].output_activation = +(float) rand() / (float) RAND_MAX;
        net->node_s[i].output_charge     = +(float) rand() / (float) RAND_MAX;
    }
    //connect Nodes randomly
    for(int i=0; i<net->node_Count; i++){
        //setup node
        struct NODE* node = &(net->node_s[i]);
        node->con_count  = io_ratio;
        node->con_weight = (float*)  malloc(sizeof(float)  * io_ratio);
        node->con_prt    = (float**) malloc(sizeof(float*) * io_ratio);
        //do random connection
        for(int j=0; j<io_ratio; j++){
            int rnd = rand() % nodes;
            node->con_prt[j] = &(net->node_s[rnd].accum);
            node->con_weight[j] = (float) rand() / (float) RAND_MAX;
        }
    }
    //allocate injectors / extractors
    net->ext_Count = extractors;
    net->inj_Count = injectors;
    net->ext_s = malloc(sizeof(float)  * extractors);
    net->inj_s = malloc(sizeof(float*) * injectors);
    if(net->ext_s == 0 || net->inj_s == 0) return 0;
    //setup injectors
    for(int i=0; i<net->inj_Count; i++){
        int rnd = rand() % net->node_Count;
        net->inj_s[i] = &(net->node_s[rnd].accum);
    }
    //exit
    return net;
}

struct NETWORK* net;
float avg_batch_energy = 1.0f;
int batch_amount;

void *network_execute(){
    int nodes = net->node_Count;
    avg_batch_energy = 0.0f;
    while(batch_amount > 0){  
        float batch_energy = 0.0f;
        batch_amount--;

        for(int i=0; i<nodes; i++){
            struct NODE* node = &(net->node_s[i]);
            if(node->accum > node->output_activation){
                batch_energy = batch_energy + node->output_charge;
                for(int j=0; j<node->con_count; j++){
                    float* target = node->con_prt[j];
                    (*target) += node->output_charge * node->con_weight[j];
                }
                node->accum = 0.0f;
            }
        }
        if(avg_batch_energy == 0){
            avg_batch_energy = batch_energy;
        }else{
            avg_batch_energy = (avg_batch_energy + batch_energy) / 2;
        }
    }
}

pthread_t network;

float network_run(struct NETWORK* netptr, int batchamount){
    net = netptr;
    batch_amount = batchamount;
    pthread_create(&network, NULL, network_execute, NULL);
    pthread_join(network, NULL);
    //pthread_detach(network);
    return avg_batch_energy;
}

void network_mutate(struct NETWORK* parten, struct NETWORK* child, float mut){
    for(int i=0; i<child->node_Count; i++){
        struct NODE* node_child  = &(child->node_s[i]);
        struct NODE* node_parent = &(parten->node_s[i]);
        //copy vals
        node_child->accum = 0.0f;
        node_child->output_activation = node_parent->output_activation  + ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);
        node_child->output_charge     = node_parent->output_charge      + ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);
        //copy conections
        for(int j=0; j<node_child->con_count; j++){

            node_child->con_weight[j] = node_parent->con_weight[j] * ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);
            float* goal = node_parent->con_prt[j];
            int index;

            for(int l=0; l<node_parent->con_count; l++){
                float* y = &(parten->node_s[l].accum);
                if(goal == y){
                    index = l;
                    break;
                }
            }
            node_child->con_prt[j] = &(child->node_s[index].accum);
        }
    }
}