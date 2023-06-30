#include "sdb.h"
#define BREAKPOINTSIZE 10
BP bp_pool[BREAKPOINTSIZE] = {};

int inser_bp(uint64_t addr)
{
    for(int i=0;i<BREAKPOINTSIZE;i++)
    {
        if(bp_pool[i].is_used==0)
        {
            bp_pool[i].addr=addr;
            bp_pool[i].is_used=1;
            return 1;
        }
    }
    return 0;
}