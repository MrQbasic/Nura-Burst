typedef struct __attribute__ ((aligned (16))) Node_connection {
    int source;
    int destination;
    float weight;
    int padding;
} Node_connection;

__kernel void burst(__global float* buf1, __global float* buf2, __global Node_connection* conn, int connectionsPerNetwork, int nodesPerNetwork){
    int connection_id = get_global_id(0);
    int network_id = get_global_id(1);

    Node_connection c = conn[network_id * connectionsPerNetwork + connection_id];

    float input = buf2[c.source];

    buf1[c.destination] += input * c.weight;
}