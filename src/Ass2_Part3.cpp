/************************************************************************************************
 * File Name: Ass2_Part3.cpp                                                                    *
 * Author: Group-5 (Swapnil, Jatin)                                                             *
 * Description: Calculate the access distance Access distance filtered by LRU cache and compute *
 *               the cumulative density function of access distances from the traces.           *
 *                                                                                              *
 ***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Defining cache configuration: sets and ways
#define SETS 2048
#define WAYS 16

//Declaring Cache, TimeStamp variables
unsigned long long Time_Stamp;
unsigned long long Cache[SETS][WAYS];
unsigned long long int Time_Stamp_Cache[SETS][WAYS];

//return the way from the given set which matches tag, else return -1
int getCacheSetWay(int cacheSet, int cacheTag)
{
    //searching all the ways in cache which matches with tag and is valid
    for (int i = 0; i < WAYS; i++)
    {
        if ((Cache[cacheSet][i] == cacheTag) && (Time_Stamp_Cache[cacheSet][i] != 0))
            return i;
    }
    return -1;
}

// Extract the tag and set from the given addr
void extract_TagSet(unsigned long long addr, unsigned long long *tag, unsigned long long *set)
{
    *set = (addr & 0x1FFC0) >> 6;
    *tag = (addr & 0xFFFFFFFFFFFE0000) >> 17;
}

//Update LRU cache Timestamp, if opr == 0 invalidate else update the TimeStamp
void updateLRUCacheTimeStamp(int cacheSet, int cacheSetWay, int opr)
{
    Time_Stamp++;
    //opr 0 mean invalidate
    if (opr == 0){
        Time_Stamp_Cache[cacheSet][cacheSetWay] = 0;
    }
    else{
        //update timestamp
        Time_Stamp_Cache[cacheSet][cacheSetWay] = Time_Stamp;
    }
}

//search timestamp and return the LRU victim way 
int getLRUVictimWay(int cacheSet)
{
    int i, min = Time_Stamp_Cache[cacheSet][0], cacheSetWay = 0;
    for (i = 1; i < WAYS; i++)
    {
        //find the way having minimum timestamp i.e. LRU 
        if (Time_Stamp_Cache[cacheSet][i] <= min)
        {
            min = Time_Stamp_Cache[cacheSet][i];
            cacheSetWay = i;
        }
    }
    return cacheSetWay;
}

//Insert the value in the cache and update the timestamp
void cacheInsert(int cacheSet, int cacheTag)
{
    //WAY return the position to be evict, then insert data at WAY and update LRU table
    int cacheSetWay = getLRUVictimWay(cacheSet);
    Cache[cacheSet][cacheSetWay] = cacheTag;
    updateLRUCacheTimeStamp(cacheSet, cacheSetWay, 1);
}

//Main Function
int main(int argc, char *argv[])
{
    FILE *fp , * trace;
    char inputFileName[100], outputFileName[100];
    unsigned long long addr, cacheTag, cacheSet, cacheSetWay;
    int threadid;
    unsigned long long cacheHitCount = 0, cacheMissCount = 0;

    //Open the input trace file 
    sprintf(inputFileName, "%s", argv[1]);
    fp = fopen(inputFileName, "r");

    //Open the output file to store miss trace
    sprintf(outputFileName, "%s_MissTrace", argv[1]);
    trace = fopen(outputFileName, "w");

    //Checking error during file read
    if (fp == NULL)
    {
        printf("Error!");
        exit(1);
    }

    while (!feof(fp))
    {
        //Read the thread id and addr from the inpu trace
        fscanf(fp, "%d  %llx", &threadid, &addr);
        
        //extract tag and set from the given addr
        extract_TagSet(addr, &cacheTag, &cacheSet);

        //Find the way of the given set and tag in cache
        cacheSetWay = getCacheSetWay(cacheSet, cacheTag);
        
        //WAY return -1 if not present in Cache
        if (cacheSetWay != -1)
        {
            //Cache Hit
            cacheHitCount++;
            //update lru timestamp
            updateLRUCacheTimeStamp(cacheSet, cacheSetWay, 1);
        }
        else
        {
            //Cache Miss
            cacheMissCount++;
            fprintf(trace,"%d  %llx \n",threadid, addr);
            
            //Insert into cache
            cacheInsert(cacheSet, cacheTag);
        }
    }

    //Print total cache hit and miss count
    printf("%lld  %lld", cacheHitCount, cacheMissCount);
    
    return 0;
}