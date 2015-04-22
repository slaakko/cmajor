/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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

static int allocation_serial = 0;
static int watch_allocation = -1;
static int free_disabled = 0;

typedef struct Alloc { void* mem; unsigned long long size; int serial; int allocated; struct Alloc* next; } Allocation;

void print_alloc(Allocation* alloc)
{
    fprintf(stderr, "serial=%d, mem=%p, size=%llu\n", alloc->serial, alloc->mem, alloc->size);
    fflush(stderr);
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

void dbgheap_watch(int serial)
{
    watch_allocation = serial;
}

void dbgheap_enable_free()
{
    free_disabled = 0;
}

void dbgheap_disable_free()
{
    free_disabled = 1;
}

const char* alive = "alive";
const char* freed = "freed!";

void dbgheap_print_info(void* mem)
{
    int index = hash(mem);
    Allocation* alloc = dbgheap[index];
    while (alloc)
    {
        if (alloc->mem == mem)
        {
            fprintf(stderr, "DBGHEAP> object %p: serial=%d, object is %s\n", mem, alloc->serial, (alloc->allocated ? alive : freed));
            fflush(stderr);
            return;
        }
        alloc = alloc->next;
    }
    fprintf(stderr, "DBGHEAP> object %p not found\n", mem);
    fflush(stderr);
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
	    fprintf(stderr, "DBGHEAP> memory leaks detected...\n");
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
    fflush(stderr);
}

void* dbgheap_malloc(unsigned long long size)
{
    if (dbgheap == NULL)
    {
        return malloc(size);
    }
    if (size >= 8192)
    {
        fprintf(stderr, "DBGHEAP> allocation size >= 8192 (%ull), serial=%d\n", size, allocation_serial);
        fflush(stderr);
    }
    if (watch_allocation == allocation_serial)
    {
        begin_capture_call_stack();
        const char* call_stack = capture_call_stack();
        fprintf(stderr, "DBGHEAP> allocating serial=%d:\ncall stack:\n%s", allocation_serial, call_stack);
        fflush(stderr);
        end_capture_call_stack();
    }
    void* mem = malloc(size);
    int index = hash(mem);
    Allocation* next = dbgheap[index];
    Allocation* alloc = (Allocation*)malloc(sizeof(Allocation));
    alloc->mem = mem;
    alloc->size = size;
    alloc->serial = allocation_serial++;
    alloc->allocated = 1;
    alloc->next = next;
    dbgheap[index] = alloc;
    return mem;
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
        if (watch_allocation == alloc->serial)
        {
            begin_capture_call_stack();
            const char* call_stack = capture_call_stack();
            fprintf(stderr, "DBGHEAP> deallocating serial=%d:\ncall stack:\n%s", alloc->serial, call_stack);
            fflush(stderr);
            end_capture_call_stack();
        }
        if (!alloc->allocated)
        {
            begin_capture_call_stack();
            const char* call_stack = capture_call_stack();
            fprintf(stderr, "DBGHEAP> double free detected:\n");
            print_alloc(alloc);
            fprintf(stderr, "\ncall stack:\n%s", call_stack);
            fflush(stderr);
            end_capture_call_stack();
            exit(1);
        }
        else
        {
            alloc->allocated = 0;
        }
    }
    if (!free_disabled)
    {
        free(mem);
    }
}
