__kernel void
add (__global float *a,
	 __global float *b,
	 __global float *ans)
{
	int gid = get_global_id(0);
	
	ans[gid] = a[gid] + b[gid];
}