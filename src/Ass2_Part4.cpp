/************************************************************************************************
 * File Name: Ass2_Part4.cpp                                                                    *
 * Author: Group-5 (Swapnil, Jatin)                                                             *
 * Description: compute the number of memory blocks that are private, or shared by two threads, *
 * or shared by three threads, ..., or shared by eight threads.                                 *
 *                                                                                              *
 ***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include<bitset>
using namespace std;

#define THREAD_COUNT 8

//Typedef the map<ull,bitset>  
typedef std::map<unsigned long long, std::bitset<8> > AddrMapType;
typedef std::map<unsigned long long, std::bitset<8> >::iterator AddrMapTypeItr;

//Main function
int main(int argc, char *argv[])
{
    FILE *fp;
    char inputFileName[100];
    unsigned long long addr, blockAddr;
    unsigned long long accessCount = 0, accessDistance = 0, blockAccessHit = 0;
    int threadid;

    //Map to store access of memory blocks
    AddrMapType blockThreadAccess, accessDistanceCount;
    AddrMapTypeItr it_find, it_print;
    
    //Delaring a bitset of 8 bits to store the corresponding thread
    std::bitset<8> blockThread;
    
    //store the count of thread sharing 
    unsigned long long threadProfile[THREAD_COUNT+1] = {0};

    sprintf(inputFileName, "%s", argv[1]);
    fp = fopen(inputFileName, "r");
    //Checking error during file read
    if (fp == NULL)
    {
        printf("Error!");
        exit(1);
    }
    //Reading the file
    while (!feof(fp))
    {
        //Reading the thread
        fscanf(fp, "%d  %llx\n", &threadid, &addr);

        //get the block addr by left shifting addr by 6 bits
        blockAddr = addr >> 6;
        
        //Reset the bitset i.e. set all bits to 0
        blockThread.reset();

         //Checks if block entry is added in the map, it returns the iterator to the block if present
        //else return the end of the iterator 
        it_find = blockThreadAccess.find(blockAddr);
        if (it_find != blockThreadAccess.end())
        {
            //get the bitset of block from the map
            blockThread = it_find->second ;

            //set the bit corresponding to threadid in bitset
            blockThread.set(threadid);
            
            //Store the updated bitset in the map
            blockThreadAccess[blockAddr] = blockThread;
        }
        else
        {
            //block entry is not in the map
            //set the bitset blockThread corresponding to the thread id
            blockThread.set(threadid);

            //Store the bitset in the map
            blockThreadAccess[blockAddr] = blockThread;
        }

    }

    //generate thread profile i.e. number of memory block that is shared by 1,2,...,8 threads
    for (it_print = blockThreadAccess.begin(); it_print != blockThreadAccess.end(); it_print++)
    {
        //it_print->second return the bitset then bitset.count() gives the number of bit is set
        // i.e. shared by number of threads. Increment the threadProfile[NumberSharedThreads] value 
        threadProfile[it_print->second.count()]++;
    }
    
    cout<<"\n******Thread Profiling********\n";
    for( int i=1; i < THREAD_COUNT+1 ; i++ )
    {
        cout<<i<<" :"<<threadProfile[i]<<endl;
    }
    return 0;
}
