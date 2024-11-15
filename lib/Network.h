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
    //alloc and check
    net->node_Count = nodes;
    net->node_s     = malloc(sizeof(struct NODE) * nodes);
    net->ext_Count  = extractors;
    net->ext_s      = malloc(sizeof(float ) * extractors);
    net->inj_Count  = injectors;
    net->inj_s      = malloc(sizeof(float*) * injectors);
    if(net->ext_s == 0 || net->inj_s == 0) return 0;
    if(net == 0) return 0;
    //initalize nodes with random parameters
    //--setup random activation + output power
    for(int i=0; i<nodes; i++){
        net->node_s[i].accum = 0.0f;
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
            int rnd = rand() % nodes + net->ext_Count;
            if(rnd < nodes){
                node->con_prt[j] = &(net->node_s[rnd].accum);
            }else{
                node->con_prt[j] = net->ext_s + (rnd-nodes);
            }
            node->con_weight[j] = (float) rand() / (float) RAND_MAX;
        }
    }
    //setup injectors
    for(int i=0; i<net->inj_Count; i++){
        int rnd = rand() % net->node_Count;
        net->inj_s[i] = &(net->node_s[rnd].accum);
    }

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

void network_mutate(struct NETWORK* parent, struct NETWORK* child, float mut){
    //sync nodes
    for(int i=0; i<child->node_Count; i++){
        struct NODE* node_child  = &(child->node_s[i]);
        struct NODE* node_parent = &(parent->node_s[i]);
        //copy vals
        node_child->accum = 0.0f;
        node_child->output_activation = node_parent->output_activation  + ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);
        node_child->output_charge     = node_parent->output_charge      + ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);
        //copy conections
        for(int j=0; j<node_child->con_count; j++){

            node_child->con_weight[j] = node_parent->con_weight[j] + ((((float) rand() / (float) RAND_MAX * 2) - 1) * mut);


            float* goal = node_parent->con_prt[j];
            int index = -1;
            int normalNode = 0;

            //check if there exists a node with the same pointer
            for(int l=0; l<parent->node_Count; l++){
                float* y = &(parent->node_s[l].accum);
                if(goal == y){
                    index = l;
                    normalNode = 1;
                    break;
                }
            }

            //check if we need to find a matching extractor ptr
            if(index == -1){
                for(int l=0; l<parent->ext_Count; l++){
                    float* y = &(parent->ext_s[l]);
                    if(goal == y){
                        index = l;
                        break;
                    }
                }
            }

            //mutate the connections

            if(((float) rand() / (float) RAND_MAX) < mut){
                index = rand();
            }
            index = index % (parent->node_Count + parent->ext_Count);

            //check if we are talking about a extractor or normal node -- then set the child
            if(normalNode == 1) {
               node_child->con_prt[j] = &(child->node_s[index].accum);
            }else{
                node_child->con_prt[j] = &(child->ext_s[index]);
            }
        }
    }
    //sync injectors
    for(int i=0; i<child->inj_Count; i++){
        int index = -1;
        for(int j=0; j<parent->node_Count; j++){
            if(&(parent->node_s[j].accum) == parent->inj_s[i]){
                index = j;
                break;
            }
        }
        if(index == -1){
            printf("Unexpected error while doing mutation");
        }
        child->inj_s[i] = &(child->node_s[index].accum);
    }

    //clean extractors
    for(int i=0; i<child->ext_s[i]; i++){
        child->ext_s[i] = 0;
        parent->ext_s[i] = 0;
    }

}