#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SETS 2048
#define WAYS 16
FILE * trace;
unsigned long long Time_Stamp;
unsigned long long Hit, Miss;
unsigned long long Cache[SETS][WAYS];
unsigned long long int Time_Stamp_Cache[SETS][WAYS];
int SEARCH_IN(int SET, int TAG)
{
    for (int i = 0; i < WAYS; i++)
    {
        if ((Cache[SET][i] == TAG) && (Time_Stamp_Cache[SET][i] != 0))
            return i;
    }
    return -1;
}
void extract_TagSet(unsigned long long addr, unsigned long long *tag, unsigned long long *set)
{
    *set = (addr & 0x1FFC0) >> 6;
    *tag = (addr & 0xFFFFFFFFFFFE0000) >> 17;
}
//Update L3 LRU Timestamp
void LRU_UPDATE_Cache_IN(int SET, int WAY, int opr)
{
    Time_Stamp++;
    //opr 0 mean invalidate
    if (opr == 0)
        Time_Stamp_Cache[SET][WAY] = 0;
    else
        //update timestamp
        Time_Stamp_Cache[SET][WAY] = Time_Stamp;
}
void INSERT_Cache_IN(int SET, int TAG)
{
    //WAY return the position to be evict, then insert data at WAY and update LRU table
    int WAY = LRU_QUERY_Cache_IN(SET);
    Cache[SET][WAY] = TAG;
    LRU_UPDATE_Cache_IN(SET, WAY, 1);
}
int LRU_QUERY_Cache_IN(int SET)
{
    int i, min = Time_Stamp_Cache[SET][0], WAY = 0;
    for (i = 1; i < WAYS; i++)
    {
        if (Time_Stamp_Cache[SET][i] <= min)
        {
            min = Time_Stamp_Cache[SET][i];
            WAY = i;
        }
    }
    return WAY;
}
int main(int argc, char *argv[])
{
    FILE *fp;
    char input_name[100];
    char output_name[100];
    unsigned long long addr, Block_addr, i = 0,TAG,SET,WAY;
    int threadid;
    sprintf(input_name, "%s", argv[1]);
    fp = fopen(input_name, "r");
    sprintf(output_name, "%s_MissTrace", argv[1]);
    trace = fopen(output_name, "w");
    //Checking error during file read
    if (fp == NULL)
    {
        printf("Error!");
        exit(1);
    }
    while (!feof(fp))
    {
        fscanf(fp, "%d  %llx", &threadid, &addr);
        
        i++;
        extract_TagSet(addr, &TAG, &SET);
        //search in Cache
        WAY = SEARCH_IN(SET, TAG);
        //WAY return -1 if not present in Cache
        if (WAY != -1)
        {
            //Cache Hit
            Hit++;
            //update lru  timestamp
            LRU_UPDATE_Cache_IN(SET, WAY, 1);
        }
        else
        {
            //Cache Miss
            Miss++;
            fprintf(trace,"%d  %llx \n",threadid, addr);
            //-------increment no of misses for this access
            

            INSERT_Cache_IN(SET, TAG);
        }
    }
    printf("%lld  %lld",Miss,Hit);
    
    return 0;
}