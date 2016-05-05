__kernel void prime(__global ulong *Number, __global bool *Output)
{
	int idx = get_global_id(0);
	if(Number[0]%idx!=0)
		Output[idx]=true;
	else
		Output[idx]=false;
}