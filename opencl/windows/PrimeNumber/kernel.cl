//__kernel void prime(__global ulong *Number, __global ulong *Output, __global bool *Status)
//{
//	int idx = get_global_id(0);
//	if(Number[0]%idx!=0)
//		Output[idx]=true;
//	else{
//		Output[idx]=false;
//		Status[0]=false;
//	}
//}

__kernel void prime(__global ulong *Number, __global int *WorkGroups, __global ulong *Output, __global int *Status)
{
	int idx = get_global_id(0);
	ulong temp = 0;
	ulong i = 0;
	ulong result = 0;
	while((temp = (*WorkGroups*i+idx)+2)<*Number){
		if(Number[0]%temp==0)
			result++;
		++i;
	}
	Output[idx]=result;

}

