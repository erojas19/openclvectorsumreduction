__kernel void sum(__global float *g_idata, __global float *sdata, __global float *g_odata) {

    int globalID = get_global_id(0);
    int localID = get_local_id(0);
    int localSize = get_local_size(0);
    int workgroupID = globalID / localSize;
    sdata[localID] = g_idata[globalID];
    barrier(CLK_LOCAL_MEM_FENCE);
    int halfBlockSize=localSize/2;
    while(halfBlockSize>0){
        if (localID < halfBlockSize ) {
            sdata[localID] =sdata[localID]+ sdata[localID + halfBlockSize];
            if((halfBlockSize*2)<localSize){
                if (localID == 0){
                 sdata[localID] =sdata[localID]+ sdata[localID + (localSize-1)];
                }
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        localSize=halfBlockSize;
        halfBlockSize=halfBlockSize/2;
    }
    if (localID == 0) {
        g_odata[0] = sdata[0];
    }
}
