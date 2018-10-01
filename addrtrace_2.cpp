/*
 *  Group-5 
 *  This file contains an ISA-portable PIN tool for ACA assignment 2-I.
 */

#include <stdio.h>
#include "pin.H"


FILE * trace;
PIN_LOCK pinLock;

VOID getMachineAddress(unsigned long long opAddr, UINT32 opSize, THREADID threadid)
{
    
    unsigned long blockBoundary = opAddr | 0x000000000000003F;
    // printf("blockOffest: %d  blockBoundary: %llx ", blockOffest, blockBoundary);
    
    while (opSize > 0)
    {
        while ((opAddr + 7) <= blockBoundary && opSize >= 8)
        {
            fprintf(trace,"%d  %llx \n",threadid, opAddr);
            opAddr += 8;
            opSize -= 8;
           
        }

        while ((opAddr + 3) <= blockBoundary && opSize >= 4)
        {
            fprintf(trace,"%d  %llx \n",threadid, opAddr);
            opAddr += 4;
            opSize -= 4;
           
        }

        while ((opAddr + 1) <= blockBoundary && opSize >= 2)
        {
            fprintf(trace,"%d  %llx \n",threadid, opAddr);
            opAddr += 2;
            opSize -= 2;
           
        }

        while ((opAddr) <= blockBoundary && opSize >= 1)
        {
            fprintf(trace,"%d  %llx \n",threadid, opAddr);
            opAddr += 1;
            opSize -= 1;
        }

        if (opSize > 0)
            blockBoundary += 64;
    }
   
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, UINT32 size, THREADID threadid)
{
    PIN_GetLock(&pinLock, threadid+1);
    unsigned long long addr1 = (unsigned long long)addr;
    getMachineAddress(addr1,size,threadid);
    fflush(trace);
    PIN_ReleaseLock(&pinLock);
}


// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.

   
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        UINT32 opSize = INS_MemoryOperandSize(ins, memOp);
        if (INS_MemoryOperandIsRead(ins, memOp))
        {   
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemAccess,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_UINT32, opSize,
                IARG_THREAD_ID,
                IARG_END);
        }
        else (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemAccess,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_UINT32, opSize,
                IARG_THREAD_ID,
                IARG_END);
        }
    }

}

VOID Fini(INT32 code, VOID *v)
{
    // fprintf(trace, "#eof\n");
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("addrtrace_mac1", "w");

    INS_AddInstrumentFunction(Instruction, 0);


    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}