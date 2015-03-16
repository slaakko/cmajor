/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include "dbgheap.h"
#include <stdio.h>
#include <string.h>
#if defined(__linux) || defined(__posix) || defined(__unix)
#include <unistd.h>
#endif

void begin_capture_call_stack();
void end_capture_call_stack();
const char* capture_call_stack();

#define ALLOC_HASH_TABLE_SIZE 196613

static int allocationNumber = 0;
static int stopOnAllocation = -1;

typedef struct Alloc { void* mem; unsigned long long size; int serial; int allocated; const char* file; int line; struct Alloc* next; } Allocation;

static char alloc_line[1024];

void print_alloc(Allocation* alloc)
{
    snprintf(alloc_line, sizeof(alloc_line) - 1, 
        "serial=%d, mem=%p, size=%llu, file=%s, line=%d\n", 
        alloc->serial, alloc->mem, alloc->size, alloc->file, alloc->line);
    write(2, alloc_line, strlen(alloc_line));
}

static Allocation** dbgheap = NULL;

int hash(void* mem)
{
    return (int)(((unsigned long long)mem) % (unsigned long long)ALLOC_HASH_TABLE_SIZE);
}

void dbgheap_init()
{
    int i;

    dbgheap = (Allocation**)malloc(ALLOC_HASH_TABLE_SIZE * sizeof(Allocation*));
    for (i = 0; i < ALLOC_HASH_TABLE_SIZE; ++i)
    {
        dbgheap[i] = NULL;
    }
}

void dbgheap_done()
{
    int i;

    for (i = 0; i < ALLOC_HASH_TABLE_SIZE; ++i)
    {
        Allocation* alloc = dbgheap[i];
        while (alloc)
        {
            Allocation* a = alloc;
            alloc = alloc->next;
            free(a);
        }
        dbgheap[i] = NULL;
    }
    free(dbgheap);
    dbgheap = NULL;
}

void dbgheap_stop(int allocation)
{
    stopOnAllocation = allocation;
}

int dbgheap_get_serial(void* mem)
{
    int index = hash(mem);
    Allocation* alloc = dbgheap[index];
    while (alloc)
    {
        if (alloc->mem == mem)
        {
            return alloc->serial;
        }
        alloc = alloc->next;
    }
    return -1;
}

int compare_alloc(const void* left, const void* right)
{
    Allocation** leftLeak = (Allocation**)left;
    Allocation** rightLeak = (Allocation**)right;
    return (*leftLeak)->serial - (*rightLeak)->serial;
}

void dbgheap_report()
{
    int i;
    int numLeaks;
    Allocation** leaks;
    int leak;

    numLeaks = 0;
    for (i = 0; i < ALLOC_HASH_TABLE_SIZE; ++i)
    {
        Allocation* alloc = dbgheap[i];
        while (alloc)
        {
            if (alloc->allocated)
            {
                ++numLeaks;
            }
            alloc = alloc->next;
        }
    }
    if (numLeaks > 0)
    {
	    write(2, "DBGHEAP: memory leaks detected...\n", strlen("DBGHEAP: memory leaks detected...\n"));
        leaks = (Allocation**)malloc(numLeaks * sizeof(Allocation*));
        leak = 0;
        for (i = 0; i < ALLOC_HASH_TABLE_SIZE; ++i)
        {
            Allocation* alloc = dbgheap[i];
            while (alloc)
            {
                if (alloc->allocated)
                {
                    leaks[leak++] = alloc;
                }
                alloc = alloc->next;
            }
        }
        qsort(leaks, numLeaks, sizeof(Allocation*), compare_alloc);
        for (i = 0; i < numLeaks; ++i)
        {
            Allocation* leak = leaks[i];
            print_alloc(leak);
        }
        free(leaks);
    }
}


void* dbgheap_malloc(unsigned long long size, const char* file, int line)
{
    if (dbgheap == NULL)
    {
        return malloc(size);
    }

    if (stopOnAllocation == allocationNumber)
    {
        begin_capture_call_stack();
        const char* callStack = capture_call_stack();
        char* buf = (char*)malloc(65536);
        snprintf(buf, 65535, "DBGHEAP> stop: serial=%d, file=%s, line=%d, %s", allocationNumber, file, line, callStack);
        write(2, buf, strlen(buf));
        free(buf);
        end_capture_call_stack();
        exit(255);
        return NULL;
    }
    else
    {
        void* mem = malloc(size);
        int index = hash(mem);
        Allocation* next = dbgheap[index];
        Allocation* alloc = (Allocation*)malloc(sizeof(Allocation));
        alloc->mem = mem;
        alloc->size = size;
        alloc->serial = allocationNumber++;
        alloc->allocated = 1;
        alloc->file = file;
        alloc->line = line;
        alloc->next = next;
        dbgheap[index] = alloc;
        return mem;
    }
}

void dbgheap_free(void* mem)
{
    if (dbgheap == NULL)
    {
        free(mem);
        return;
    }

    int index = hash(mem);
    Allocation* alloc = dbgheap[index];
    while (alloc && alloc->mem != mem)
    {
        alloc = alloc->next;
    }
    if (alloc != NULL)
    {
        if (!alloc->allocated)
        {
            write(2, "DBGHEAP> double free detected:\n", strlen("DBGHEAP> double free detected:\n"));
            print_alloc(alloc);
            begin_capture_call_stack();
            const char* callStack = capture_call_stack();
            write(2, callStack, strlen(callStack));
            end_capture_call_stack();
            exit(255);
        }
        else
        {
            alloc->allocated = 0;
        }
    }
    free(mem);
}

