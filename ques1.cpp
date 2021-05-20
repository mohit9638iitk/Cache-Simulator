#include<iostream>
#include<bits/stdc++.h>
#include<fstream>

using namespace std;

#define l2_set 1024
#define l2_associativity 8
#define l2_block_size 64

#define l3_set 2048
#define l3_associativity 16
#define l3_block_size 64


#define ull unsigned long long 
class cache{
	public:
		bool valid;
		unsigned long long tag;
		unsigned long long counter;
		cache()
		{
			valid=false;
			tag=-1;
			counter	=-1;
		}
};	



void space_free(cache** &C,int num_of_set,int associativity,int block_size)
{
	for(int i=0;i<associativity;i++)
	{
	      delete C[i];
	}
	delete C;
       return ;
}

void  space_acquire(cache**&C,int num_of_set,int associativity,int block_size)
{

	C=new cache*[num_of_set*sizeof(cache*)];
	for(int i=0;i<num_of_set;i++)
	{

		C[i]=new cache[associativity*sizeof(cache)];
	}
	return ;
}

int count_bits(int x)
{
	int ans=0;
	while(x>0)
	{ 
   	   x>>=1;
	   ans++;
        }
    return ans-1;
}
bool back_inv(cache**&C,int num_of_set,int associativity,int block_size,ull address)
{
	int add_64[64]={0};
	int count=0;
	ull value=address;
	while(address>0)
	{
		add_64[count]=(address&1);
		count++;
		address>>=1;
	}
	int block_offset_bits=count_bits(block_size);
	int index_bits=count_bits(num_of_set);
	int index=0;
	long long  ans=1;
	for(int i=block_offset_bits;i<(index_bits+block_offset_bits);i++)
	{
		index+=(add_64[i]*ans);
		ans<<=1;
	}
        
	ull tag=0;
	ans=1;
	for(int i=index_bits+block_offset_bits;i<64;i++)
	{
	    tag+=(add_64[i]*ans);
	    ans<<=1;
	}
        //now we have index and tag . Tag contain block address.index contain the set number
	for(int i=0;i<associativity;i++)
	{
		if((C[index][i].valid==true)&&(C[index][i].tag==tag))
		{
			C[index][i].valid=false;
			return true;	
		}
	}
	return false;


}
int lru_replacement(cache * &set_index,int associativity)
{
	for(int i=0;i<associativity;i++)
	{
		if(!set_index[i].valid)
		{
			return i;
		}
	}
	int index=0;
        for(int i=0;i<associativity;i++)
	{
		if(set_index[i].counter<set_index[index].counter)
		{
			index=i;
		}
	}
	return index;
}
bool cache_present(cache ** &C,int num_of_set, int associativity, int block_size, ull address, ull current_counter)
{
	int add_64[64]={0};
	int count=0;
	ull value=address;
	while(address>0)
	{
		add_64[count]=(address&1);
		count++;
		address>>=1;
	}
	int block_offset_bits=count_bits(block_size);
	int index_bits=count_bits(num_of_set);
	int index=0;
	long long  ans=1;
	for(int i=block_offset_bits;i<(index_bits+block_offset_bits);i++)
	{
		index+=(add_64[i]*ans);
		ans<<=1;
	}
        
	ull tag=0;
	ans=1;
	for(int i=index_bits+block_offset_bits;i<64;i++)
	{
	    tag+=(add_64[i]*ans);
	    ans<<=1;
	}
	
	for(int i=0;i<associativity;i++)
	{
		if((C[index][i].valid==true)&&(C[index][i].tag==tag))
		{
			C[index][i].counter=current_counter;
			return true;
		}
	}
	return false;
	
}

ull cache_handler_miss(cache ** &C,int num_of_set,int associativity,int block_size,ull address, ull current_counter)
{
	int add_64[64]={0};
	int count=0;
	ull value=address;
	while(address>0)
	{
		add_64[count]=(address&1);
		count++;
		address>>=1;
	}
	int block_offset_bits=count_bits(block_size);
	int index_bits=count_bits(num_of_set);
	int index=0;
	long long  ans=1;
	for(int i=block_offset_bits;i<(index_bits+block_offset_bits);i++)
	{
		index+=(add_64[i]*ans);
		ans<<=1;
	}
        
	ull tag=0;
	ans=1;
	for(int i=index_bits+block_offset_bits;i<64;i++)
	{
	    tag+=(add_64[i]*ans);
	    ans<<=1;
	}
	
	ull evicted_address,evicted_tag;
	
	int replace_line=lru_replacement(C[index],associativity);
	if(!C[index][replace_line].valid)
	{
		evicted_address=-1;
	}
	else
	{
		evicted_tag=C[index][replace_line].tag;
		evicted_address=(evicted_tag*num_of_set+index)*(block_size);
	}
	C[index][replace_line].tag=tag;
	C[index][replace_line].valid=true;
	C[index][replace_line].counter=current_counter;

	return evicted_address;
	
}

void inclusive(int argc,char ** argv)
{

        cache **l2;
	space_acquire(l2,l2_set,l2_associativity,l2_block_size);
	cache **l3;
	space_acquire(l3,l3_set,l3_associativity,l3_block_size);

	ull access_counter = 0;
	ull l2_miss_counter = 0;
	ull l2_hit_counter = 0;
	ull l3_miss_counter = 0;
	ull l3_hit_counter = 0;

	
    	char *type=new char;
    	char *iord=new char;
    	ull *addr=new ull;
    	unsigned* pc=new unsigned;

        for(int i=0;i<atoi(argv[2]);i++)
	{
    	FILE * fp;
	char input_name[100];
	sprintf(input_name,"../traces/%s.log_l1misstrace_%d",argv[1],i);
	fp=fopen(input_name,"rb");
	assert(fp!=NULL);

    	
    	while (!feof(fp))
    	{
    	     fread(iord, sizeof(char), 1, fp);
    	     fread(type, sizeof(char), 1, fp);
    	     fread(addr, sizeof(unsigned long long), 1, fp);
    	     fread(pc, sizeof(unsigned), 1, fp);
    	     int x=(int)*type;
    	     if(x!=0)
    	    {
    	        access_counter++;
    	        if(cache_present(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter))
    	        {
    	            l2_hit_counter++;
    	        }
    	        else
    	        {
    	            l2_miss_counter++;
    	            if(cache_present(l3,l3_set,l3_associativity,l3_block_size,*addr,access_counter))
    	            {
    	                cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
    	                l3_hit_counter++;
	
    	            }
    	            else
    	            {
    	                l3_miss_counter++;
    	                ull evicted_address = cache_handler_miss(l3,l3_set,l3_associativity,l3_block_size,*addr,access_counter);
    	                if(evicted_address != -1)
    	                {
    	                    back_inv(l2,l2_set,l2_associativity,l2_block_size,evicted_address);
    	                }
    	                cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
    	            }
    	        }
    	    }
    	}
    	fclose(fp);
	}

	space_free(l2,l2_set,l2_associativity,l2_block_size);
	space_free(l3,l3_set,l3_associativity,l3_block_size);
    	cout<<"L2_Access  "<<access_counter<<endl;
    	cout<<"L2_Hit     "<<l2_hit_counter<<endl;
	cout<<"L2_Miss    "<<l2_miss_counter<<endl;
	cout<<"L3_Hit     "<<l3_hit_counter<<endl;
	cout<<"L3_Miss    "<<l3_miss_counter<<endl;
	
	return ;
}

void exclusive(int argc,char ** argv)
{

        cache **l2;
	space_acquire(l2,l2_set,l2_associativity,l2_block_size);
	cache **l3;
	space_acquire(l3,l3_set,l3_associativity,l3_block_size);

	ull access_counter = 0;
	ull l2_miss_counter = 0;
	ull l2_hit_counter = 0;
	ull l3_miss_counter = 0;
	ull l3_hit_counter = 0;


    	char *type=new char;
    	char *iord=new char;
    	ull *addr=new ull;
    	unsigned* pc=new unsigned;


	for(int i=0;i<atoi(argv[2]);i++)
	{
    	FILE * fp;
	char input_name[100];
	sprintf(input_name,"../traces/%s.log_l1misstrace_%d",argv[1],i);
	fp=fopen(input_name,"rb");
	assert(fp!=NULL);
    	while (!feof(fp))
    	{
    	     fread(iord, sizeof(char), 1, fp);
    	     fread(type, sizeof(char), 1, fp);
    	     fread(addr, sizeof(unsigned long long), 1, fp);
    	     fread(pc, sizeof(unsigned), 1, fp);
    	     int x=(int)*type;
    	     if(x!=0)
    	    {
    	        access_counter++;
    	        if(cache_present(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter))
    	        {
    	            l2_hit_counter++;
    	        }
    	        else
    	        {
    	            l2_miss_counter++;
    	            if(cache_present(l3,l3_set,l3_associativity,l3_block_size,*addr,access_counter))
    	            {	
			back_inv(l3,l3_set,l3_associativity,l3_block_size,*addr);
    	                ull evicted_address=cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
			cache_handler_miss(l3,l3_set,l3_associativity,l3_block_size,evicted_address,access_counter);
    	                l3_hit_counter++;
	
    	            }
    	            else
    	            {
    	                l3_miss_counter++;
    	                ull evicted_address = cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
    	                cache_handler_miss(l3,l3_set,l3_associativity,l3_block_size,evicted_address,access_counter);
    	            }
    	        }
    	    }
    	}
    	fclose(fp);
	}

	space_free(l2,l2_set,l2_associativity,l2_block_size);
	space_free(l3,l3_set,l3_associativity,l3_block_size);
    	cout<<"L2_Access  "<<access_counter<<endl;
    	cout<<"L2_Hit     "<<l2_hit_counter<<endl;
	cout<<"L2_Miss    "<<l2_miss_counter<<endl;
	cout<<"L3_Hit     "<<l3_hit_counter<<endl;
	cout<<"L3_Miss    "<<l3_miss_counter<<endl;
	
	return ;
}
void nine(int argc,char ** argv)
{

        cache **l2;
	space_acquire(l2,l2_set,l2_associativity,l2_block_size);
	cache **l3;
	space_acquire(l3,l3_set,l3_associativity,l3_block_size);

	ull access_counter = 0;
	ull l2_miss_counter = 0;
	ull l2_hit_counter = 0;
	ull l3_miss_counter = 0;
	ull l3_hit_counter = 0;

	
    
 

    	char *type=new char;
    	char *iord=new char;
    	ull *addr=new ull;
    	unsigned* pc=new unsigned;

	for(int i=0;i<atoi(argv[2]);i++)
	{
    	FILE * fp;
	char input_name[100];
	sprintf(input_name,"../traces/%s.log_l1misstrace_%d",argv[1],i);
	fp=fopen(input_name,"rb");
	assert(fp!=NULL);
    	while (!feof(fp))
    	{
    	     fread(iord, sizeof(char), 1, fp);
    	     fread(type, sizeof(char), 1, fp);
    	     fread(addr, sizeof(unsigned long long), 1, fp);
    	     fread(pc, sizeof(unsigned), 1, fp);
    	     int x=(int)*type;
    	     if(x!=0)
    	    {
    	        access_counter++;
    	        if(cache_present(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter))
    	        {
    	            l2_hit_counter++;
    	        }
    	        else
    	        {
    	            l2_miss_counter++;
    	            if(cache_present(l3,l3_set,l3_associativity,l3_block_size,*addr,access_counter))
    	            {
    	                cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
    	                l3_hit_counter++;
	
    	            }
    	            else
    	            {
    	                l3_miss_counter++;
    	                cache_handler_miss(l3,l3_set,l3_associativity,l3_block_size,*addr,access_counter);
    	                cache_handler_miss(l2,l2_set,l2_associativity,l2_block_size,*addr,access_counter);
    	            }
    	        }
    	    }
    	}
    	fclose(fp);
	}

	space_free(l2,l2_set,l2_associativity,l2_block_size);
	space_free(l3,l3_set,l3_associativity,l3_block_size);
    	cout<<"L2_Access  "<<access_counter<<endl;
    	cout<<"L2_Hit     "<<l2_hit_counter<<endl;
	cout<<"L2_Miss    "<<l2_miss_counter<<endl;
	cout<<"L3_Hit     "<<l3_hit_counter<<endl;
	cout<<"L3_Miss    "<<l3_miss_counter<<endl;
	
	return ;
}

         
int main(int argc,char**argv)
{
    cout<<"-------------------------------------------------"<<endl;
    cout<<"Inclusive Policy"<<endl;
    inclusive(argc,argv);
    cout<<"-------------------------------------------------"<<endl;
    cout<<"Exclusive Policy"<<endl;
    exclusive(argc,argv);
    cout<<"-------------------------------------------------"<<endl;
    cout<<"NINE Policy"<<endl;
    nine(argc,argv);
    cout<<"-------------------------------------------------"<<endl;
    return 0;
}
