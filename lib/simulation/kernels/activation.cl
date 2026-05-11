__kernel void activation(__global float* buf1, __global float* buf2, int nodeCountTotal, int nodeCountOutput){
    float exponent = 15.0f;

    int nodeID = get_global_id(0);
    if(nodeID >= (nodeCountTotal-nodeCountOutput)) return;

    int netID = get_local_id(1);

    float input = buf1[netID * nodeCountTotal + nodeID];

    float s1 = 1.0f / (1.0f + exp((-exponent)* (input-0.5f)));
    float s2 = 1.0f / (1.0f + exp(  exponent * (input+0.5f)));

    float activationOut = s1 + s2;

    buf2[netID * nodeCountTotal + nodeID] = activationOut;

    buf1[netID * nodeCountTotal + nodeID] = input - ( fabs(activationOut) * input );
}