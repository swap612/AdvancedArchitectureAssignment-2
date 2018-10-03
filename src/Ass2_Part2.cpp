/***********************************************************************************************
 * File Name: Ass2_Part2.cpp                                                                   *
 * Author: Group-5 (Swapnil, Jatin)                                                            *
 * Description: Calculate the access distance and compute the cumulative density function of   *
 *              access distances from the traces.                                              *
 * *********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <map>
using namespace std;

//Typedef the map<ull,ull>  
typedef std::map<unsigned long long, unsigned long long> AddrMapType;
typedef std::map<unsigned long long, unsigned long long>::iterator AddrMapTypeItr;

//Main Function
int main(int argc, char *argv[])
{
    FILE *fp;
    char input_name[100];
    unsigned long long addr, blockAddr;
    unsigned long long accessCount = 0, accessDistance = 0;
    long double blockAccessHit = 0;
    int threadid;

    //Map to store last access of memory blocks and access distances
    AddrMapType lastBlockAccess, accessDistanceCount;
    
    //Iterator to iterate over the Maps
    AddrMapTypeItr it_find, it_print;
    
    sprintf(input_name, "%s", argv[1]);
    fp = fopen(input_name, "r");
   
    //Checking error during file read
    if (fp == NULL)
    {
        printf("Error!");
        exit(1);
    }
   
    //Reading the file
    while (!feof(fp))
    {
        //Read the thread_id and addr from the trace
        fscanf(fp, "%d  %llx\n", &threadid, &addr);
        
        //get the block address from the machine access
        blockAddr = addr>>6;
      
        //Checks if block entry is added in the map, it returns the iterator to the block if present
        //else return the end of the iterator   
        it_find = lastBlockAccess.find(blockAddr);
        if (it_find != lastBlockAccess.end())
        {
            blockAccessHit++;
            // calculate the accessdistance by taking difference in current access and the last access stored in map
            accessDistance = accessCount - it_find->second;
            
            //Checks if accessdistance is added in the map, it returns the iterator to the accessdistance if present
            //else return the end of the iterator   
            it_find = accessDistanceCount.find(accessDistance);
            if (it_find != accessDistanceCount.end())
            {
                //Increment the access distance count and store back in map
                accessDistanceCount[accessDistance] = it_find->second + 1;
            }
            else
            {
                //If access distance not present in Map add in map and make count 1
                accessDistanceCount[accessDistance] = 1;
            }

            //Update the map last access of the block to store the current access
            lastBlockAccess[blockAddr] = accessCount;
        }
        else
        {
            //add the access of the blockAddr in the map
            lastBlockAccess[blockAddr] = accessCount;
        }
        
        //keeps track of the access
        accessCount++;
    }

    long double cumulativeCount = 0, cumulativeFunValue = 0;
    
    //Iterate over the accessDisatnce count map to calculate cumulative function 
    for (it_print = accessDistanceCount.begin(); it_print != accessDistanceCount.end(); it_print++)
    {
        //Calculate cumulative value by adding the access distance count from map
        cumulativeCount += it_print->second;
        //calculating cumulative F(d) = n(cumulativeCount)/N(blockAccessHit)
        cumulativeFunValue = cumulativeCount/(long double)blockAccessHit;
        //Print the access distance and the comulative F(d)
        cout << endl
             << it_print->first << "," <<std::setprecision(9)<<cumulativeFunValue;
    }

    return 0;
}
