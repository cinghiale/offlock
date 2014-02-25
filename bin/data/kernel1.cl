__kernel void vector_add_gpu (__global float* vertex, const int num)
{
    /* get_global_id(0) returns the ID of the thread in execution.
    As many threads are launched at the same time, executing the same kernel,
    each one will receive a different ID, and consequently perform a different computation.*/
    const int idx = get_global_id(0);

    /* Now each work-item asks itself: "is my ID inside the vector's range?"
    If the answer is YES, the work-item performs the corresponding computation*/
    if (idx < num)
        vertex[idx] += 1;
}
