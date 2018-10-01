/************************************************************************************
 * File Name: SharingProfile.cpp                                                    *                                 *
 * Author: Swapnil     
 *         Jatin                                                             *
 *                                                                                  *
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include<bitset>
using namespace std;

#define THREAD_COUNT 8

typedef std::map<unsigned long long, std::bitset<8> > AddrMapType;
typedef std::map<unsigned long long,  std::bitset<8> >::iterator AddrMapTypeItr;

int main(int argc, char *argv[])
{
    FILE *fp;
    char input_name[100];
    unsigned long long addr, blockAddr;
    unsigned long long accessCount = 0, accessDistance = 0, blockAccessHit = 0;
    int threadid;

    //Map to store access of memory blocks
    AddrMapType blockThreadAccess, accessDistanceCount;
    AddrMapTypeItr it_find, it_print;
    std::bitset<8> blockThread;
    unsigned long long threadProfile[THREAD_COUNT+1] = {0};
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
        fscanf(fp, "%d  %llx\n", &threadid, &addr);
        blockAddr = addr>>6;
        // cout << endl
        //      << "ThreadID: "<< threadid << " : BlockAddr" << blockAddr;
        
        blockThread.reset();
        it_find = blockThreadAccess.find(blockAddr);
        if (it_find != blockThreadAccess.end())
        {
            blockThread = it_find->second ;
            blockThread.set(threadid);
            // cout << ": Block Found " << blockThread << " ";
            blockThreadAccess[blockAddr] = blockThread;
           
            
        }
        else
        {
            blockThread.set(threadid);
            // cout << ": Block Not Found " << blockThread;
            blockThreadAccess[blockAddr] = blockThread;
        }
        blockAccessHit++;
    }

    // cout<<endl;
    for (it_print = blockThreadAccess.begin(); it_print != blockThreadAccess.end(); it_print++)
    {
        // cout<<it_print->first <<" =>"<< it_print->second<< " Count"<<it_print->second.count()<<endl;
        threadProfile[it_print->second.count()]++;
    }
    
    cout<<"\n******Thread Profiling********\n";
    for( int i=1; i < THREAD_COUNT+1 ; i++ )
    {
        cout<<i<<" :"<<threadProfile[i]<<endl;
    }
    return 0;
}
