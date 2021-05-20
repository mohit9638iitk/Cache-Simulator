#include<iostream>
#include<bits/stdc++.h>
#include<fstream>

using namespace std;

#define l2_set 1024
#define l2_associativity 8
#define l2_block_size 64

#define l3_associativity 32768
#define l3_block_size 64


#define ull unsigned long long 
unordered_map<ull,bool> umap;
unordered_map<ull,queue<ull> > m;
class cache{
	public:
		bool valid;
		unsigned long long tag;
		unsigned long long counter;
		cache()
		{
			valid=false;
			tag=-1;
			counter=-1;
		}
};	

void miss(cache* &C,int associativity,ull tag)
{
	if (umap.find(tag) == umap.end())
	{
	       umap[tag] = true;
	}

}

void space_freel2(cache** &C,int num_of_set,int associativity,int block_size)
{
	for(int i=0;i<associativity;i++)
	{
	      delete C[i];
	}
	delete C;
       return ;
}
void space_freel3(cache* &C,int associativity,int block_size)
{
	delete C;
        return ;
}

void  space_acquirel2(cache**&C,int num_of_set,int associativity,int block_size)
{

	C=new cache*[num_of_set*sizeof(cache*)];
	for(int i=0;i<num_of_set;i++)
	{

		C[i]=new cache[associativity*sizeof(cache)];
	}
	return ;
}

void  space_acquirel3(cache* &C,int associativity,int block_size)
{

	C = new cache[associativity*sizeof(cache)];
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
int lru_replacementl2(cache * &set_index,int associativity)
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


int belady_replacementl3(cache* &C,int associativity)
{
	for(int i=0;i<associativity;i++)
	{
		if(!C[i].valid)
		{
			return i;
		}
		ull block_address=C[i].tag;
		if(m[block_address].empty())
		{
			return i;
		}
	}
	
	int long_int_index=0;
	int long_int=-1;
	for(int i=0;i<associativity;i++)
	{
		ull block_address=C[i].tag;
			if(m[block_address].front()>long_int)
			{
				long_int=m[block_address].front();
				long_int_index=i;
			}
		
	}
	return long_int_index;
		
}


bool cache_presentl2(cache ** &C,int num_of_set, int associativity, int block_size, ull address, ull current_counter)
{
	int add_64[64]={0};
	int count=0;
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

bool cache_presentl3(cache * &C,int associativity, int block_size, ull address, ull current_counter)
{
	int add_64[64]={0};
	int count=0;
	while(address>0)
	{
		add_64[count]=(address&1);
		count++;
		address>>=1;
	}
	int block_offset_bits=count_bits(block_size);
	ull tag=0;
	long long ans=1;
	for(int i=block_offset_bits;i<64;i++)
	{
	    tag+=(add_64[i]*ans);
	    ans<<=1;
	}
	
	for(int i=0;i<associativity;i++)
	{
		if((C[i].valid==true)&&(C[i].tag==tag))
		{
			C[i].counter=current_counter;
			return true;
		}
	}
	return false;
}


ull cache_handler_missl2(cache ** &C,int num_of_set,int associativity,int block_size,ull address, ull current_counter)
{
	int add_64[64]={0};
	int count=0;
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
	
	int replace_line=lru_replacementl2(C[index],associativity);
	if(replace_line==-1)
	{
	  cout<<"kat gya"<<endl;
	}
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


ull cache_handler_missl3(cache * &C,int associativity,int block_size,ull address, ull current_counter)
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
	ull tag=0;
	long long ans=1;
	for(int i=block_offset_bits;i<64;i++)
	{
	    tag+=(add_64[i]*ans);
	    ans<<=1;
	}
	ull evicted_address,evicted_tag;
	if(tag!=(value/block_size))
	{
		cout<<"logic fail"<<endl;
	}
	
	miss(C,associativity,tag);
	
	
	int replace_line=belady_replacementl3(C,associativity);
	if(!C[replace_line].valid)
	{
		evicted_address=-1;
	}
	else
	{
	        evicted_tag=C[replace_line].tag;
		evicted_address=(evicted_tag)*(block_size);
	}
	C[replace_line].tag=tag;
	C[replace_line].valid=true;
	C[replace_line].counter=current_counter;

	return evicted_address;
	
}

void inclusive(int argc,char ** argv)
{

    cache **l2;
    space_acquirel2(l2,l2_set,l2_associativity,l2_block_size);
    cache *l3;
    space_acquirel3(l3,l3_associativity,l3_block_size);

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
	    ull block_address=(*addr)/l3_block_size;
	    m[block_address].push(access_counter);
	}
     }
	fclose(fp);
   }
	ull temp_counter=0;
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
	    temp_counter++;
	    ull block_address=(*addr)/l3_block_size;
	    if(!m[block_address].empty())
	    {
		m[block_address].pop();
	    }
    	    if(cache_presentl2(l2,l2_set,l2_associativity,l2_block_size,*addr,temp_counter))
    	    {
    	        l2_hit_counter++;
    	    }
    	    else
    	    {
    	        l2_miss_counter++;
    	        if(cache_presentl3(l3,l3_associativity,l3_block_size,*addr,temp_counter))
    	        {
    	            cache_handler_missl2(l2,l2_set,l2_associativity,l2_block_size,*addr,temp_counter);
    	            l3_hit_counter++;
		}
    	        else
    	        {
    	            l3_miss_counter++;
    	            ull evicted_address = cache_handler_missl3(l3,l3_associativity,l3_block_size,*addr,temp_counter);
    	            if(evicted_address != -1)
    	            {
    	                back_inv(l2,l2_set,l2_associativity,l2_block_size,evicted_address);
    	            }
    	            cache_handler_missl2(l2,l2_set,l2_associativity,l2_block_size,*addr,temp_counter);
    	        }
    	    }
       }
   }
 
 fclose(fp);
}
	space_freel2(l2,l2_set,l2_associativity,l2_block_size);
	space_freel3(l3,l3_associativity,l3_block_size);
	cout<<"L3_CapacityMiss  "<<l3_miss_counter-umap.size()<<endl;
	cout<<"L3_ColdMiss      "<<umap.size()<<endl;
	
	return ;
}
     
int main(int argc,char**argv)
{ 
    inclusive(argc,argv);
    return 0;
}
