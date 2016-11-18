__kernel
void horizFlip(__global float* dest_data,
                __global float* src_data,
                __global float* metaData)
{

    const int W = metaData[0];

    const int ix = get_global_id(0);
    const int iy = get_global_id(1);

    int xpos = W - ix - 1;

    dest_data[iy*W+ix] = src_data[iy*W+xpos];
    dest_data[iy*W+xpos] = src_data[iy*W+ix];
}