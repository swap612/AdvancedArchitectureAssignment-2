#include <stdio.h>
#include <stdlib.h>
#include<iomanip>
#include <iostream>
#include <map>
using namespace std;

typedef std::map<unsigned long long, unsigned long long> AddrMapType;
typedef std::map<unsigned long long, unsigned long long>::iterator AddrMapTypeItr;

int main(int argc, char *argv[])
{
    FILE *fp;
    char input_name[100];
    unsigned long long addr, blockAddr;
    unsigned long long accessCount = 0, accessDistance = 0;
    long double blockAccessHit = 0;
    int threadid;

    //Map to store access of memory blocks
    AddrMapType lastBlockAccess, accessDistanceCount;
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
        fscanf(fp, "%d  %llx\n", &threadid, &addr);
        blockAddr = addr>>6;
        // cout << endl
        //      << accessCount << " : BlockAddr" << blockAddr;

        it_find = lastBlockAccess.find(blockAddr);
        if (it_find != lastBlockAccess.end())
        {
            blockAccessHit++;
            accessDistance = accessCount - it_find->second;
            // cout << ": Block Found " << it_find->second << " ";
            it_find = accessDistanceCount.find(accessDistance);
            if (it_find != accessDistanceCount.end())
            {
                // cout << ": Access Distance Found " << accessDistance;
                accessDistanceCount[accessDistance] = it_find->second + 1;
                // cout << ": Updated Access Dist Count " << accessDistanceCount[accessDistance];
            }
            else
            {
                // cout << ": Access Distance Not Found " << accessDistance;
                accessDistanceCount[accessDistance] = 1;
            }

            lastBlockAccess[blockAddr] = accessCount;
        }
        else
        {
            // cout << ": Block Not Found " << blockAddr;
            lastBlockAccess[blockAddr] = accessCount;
        }

        accessCount++;
    }
    // cout << "\n N is " << blockAccessHit;

    // cout<<"n +++++++Printing CDF+++++++++++";
    long double cumulativeCount=0;
    long double cumulativeFunValue;
    for (it_print = accessDistanceCount.begin(); it_print != accessDistanceCount.end(); it_print++)
    {
        cumulativeCount += it_print->second;
        cumulativeFunValue = cumulativeCount/(long double)blockAccessHit;
        cout << endl
             << it_print->first << "," <<std::setprecision(9)<<cumulativeFunValue;// << "  "<<cumulativeCount;
    }
    // for (it_print = accessDistanceCount.begin(); it_print != accessDistanceCount.end(); it_print++)
    // {
    //     cout << endl<< it_print->first << " => " << it_print->second;
    // }
    return 0;
}
