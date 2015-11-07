/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef void* thread_t;
thread_t this_thread(void);

static int numEx = 0;
typedef void* exception_addr_t;

#define THREAD_HASH_TABLE_SIZE 1009

typedef struct { const char* fun; const char* file; int line; } stack_frame;

#define CALL_STACK_SIZE 1024

typedef struct ThreadTblRec_ 
{ 
    thread_t tid; 
    int current_exception_captured; 
    int current_exception_id; 
    void* current_exception_addr; 
    void* table; 
    stack_frame* call_stack; 
    stack_frame* fp; 
    struct ThreadTblRec_* next; 
} 
ThreadTblRec;

static ThreadTblRec** threadHashTbl = 0;

static int thread_hash(thread_t tid)
{
    return (int)(((unsigned long long)tid) % (unsigned long long)THREAD_HASH_TABLE_SIZE);
}

void set_current_exception_captured()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            rec->current_exception_captured = 1;
            return;
        }
        rec = rec->next;
    }    
}

int current_exception_captured()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            return rec->current_exception_captured;
        }
        rec = rec->next;
    }    
}

int get_current_exception_id()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            return rec->current_exception_id;
        }
        rec = rec->next;
    }    
    return 0;
}

void* get_current_exception_addr()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            return rec->current_exception_addr;
        }
        rec = rec->next;
    }
    return 0;
}

void set_current_exception_id(int exception_id)
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            rec->current_exception_id = exception_id;
            return;
        }
        rec = rec->next;
    }    
}

void set_current_exception_addr(void* exception_addr)
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            rec->current_exception_addr = exception_addr;
            return;
        }
        rec = rec->next;
    }    
}

void reset_current_exception()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            rec->current_exception_id = 0;
            rec->current_exception_addr = 0;
            return;
        }
        rec = rec->next;
    }
}

void allocate_thread_data(thread_t tid);
void free_thread_data(thread_t tid);

void enter_frame(const char* fun, const char* file, int line)
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            stack_frame* fp = rec->fp;
            stack_frame* call_stack = rec->call_stack;
            if (fp < call_stack + CALL_STACK_SIZE)
            {
                fp->fun = fun;
                fp->file = file;
                fp->line = line;
            }
            ++fp;
            rec->fp = fp;
            return;
        }    
        rec = rec->next;
    }    
}

void leave_frame()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            stack_frame* fp = rec->fp;
            stack_frame* call_stack = rec->call_stack;
            if (fp > call_stack)
            {
                --fp;
                rec->fp = fp;
            }
            return;
        }    
        rec = rec->next;
    }    
}

#define CALL_STACK_BUFLEN 65536

static pthread_mutex_t call_stack_buf_lock = PTHREAD_MUTEX_INITIALIZER;
static char call_stack_buf[CALL_STACK_BUFLEN];

void begin_capture_call_stack()
{
    pthread_mutex_lock(&call_stack_buf_lock);
}

void end_capture_call_stack()
{
    pthread_mutex_unlock(&call_stack_buf_lock);
}

const char* capture_call_stack()
{
    thread_t tid = this_thread();
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            stack_frame* fp = rec->fp;
            stack_frame* call_stack = rec->call_stack;
            if (fp > call_stack)
            {
                char fun_buf[1024];
                char file_buf[1024];
                char line_buf[64];
                char frame_buf[64];
                 stack_frame* frame = fp;
                --frame;
                if (frame >= call_stack + CALL_STACK_SIZE)
                {
                    frame = call_stack + CALL_STACK_SIZE - 1;
                }
                char* p = call_stack_buf;
                strcpy(p, "call stack:\n");
                int call_stack_len = strlen(p);
                while (frame >= call_stack)
                {
                    if (frame->fun)
                    {
                        strncpy(fun_buf, frame->fun, sizeof(fun_buf) - 1);
                    }
                    else
                    {
                        fun_buf[0] = '\0';
                    }
                    if (frame->file)
                    {
                        strncpy(file_buf, frame->file, sizeof(file_buf) - 1);
                    }
                    else
                    {
                        file_buf[0] = '\0';
                    }
                    snprintf(line_buf, sizeof(line_buf) - 1, "%d", frame->line);
                    int i = frame - call_stack;
                    snprintf(frame_buf, sizeof(frame_buf) - 1, "%d", i);
                    call_stack_len += strlen("> function ' ' file  line  \n ") + strlen(fun_buf) + strlen(file_buf) + strlen(line_buf) + strlen(frame_buf);
                    if (call_stack_len >= CALL_STACK_BUFLEN)
                    {
                        break;
                    }
                    strcat(p, frame_buf);
                    strcat(p, "> ");
                    strcat(p, "function '");
                    strcat(p, fun_buf);
                    strcat(p, "' file ");
                    strcat(p, file_buf);
                    strcat(p, " line ");
                    strcat(p, line_buf);
                    strcat(p, "\n");
                    --frame;
                }
            }
            else
            {
                call_stack_buf[0] = '\0';
            }
            return call_stack_buf;
        }        
        rec = rec->next;
    }
}

void threadtbl_init(int numExceptions)
{
    int i;

    numEx = numExceptions;
    threadHashTbl = (ThreadTblRec**)malloc(THREAD_HASH_TABLE_SIZE * sizeof(ThreadTblRec*));
    for (i = 0; i < THREAD_HASH_TABLE_SIZE; ++i)
    {
        threadHashTbl[i] = 0;
    }
    allocate_thread_data(this_thread());
}

void threadtbl_done()
{
    int i;

    free_thread_data(this_thread());
    for (i = 0; i < THREAD_HASH_TABLE_SIZE; ++i)
    {
        ThreadTblRec* rec = threadHashTbl[i];
        while (rec)
        {
            ThreadTblRec* r = rec;
            rec = rec->next;
            free(r);
        }
        threadHashTbl[i] = 0;
    }
    free(threadHashTbl);
    threadHashTbl = 0;
}

void allocate_thread_data(thread_t tid)
{
    int size = numEx * sizeof(exception_addr_t);
    void* table = malloc(size);
    memset(table, 0, size);
    stack_frame* call_stack = (stack_frame*)malloc(CALL_STACK_SIZE * sizeof(stack_frame));
    memset(call_stack, 0, CALL_STACK_SIZE * sizeof(stack_frame));
    int index = thread_hash(tid);
    ThreadTblRec* next = threadHashTbl[index];
    ThreadTblRec* rec = (ThreadTblRec*)malloc(sizeof(ThreadTblRec));
    rec->tid = tid;
    rec->current_exception_captured = 0;
    rec->current_exception_id = 0;
    rec->current_exception_addr = 0;
    rec->table = table;
    rec->call_stack = call_stack;
    rec->fp = rec->call_stack;
    rec->next = next;
    threadHashTbl[index] = rec;
}

void free_thread_data(thread_t tid)
{
    int index = thread_hash(tid);
    ThreadTblRec* prev = 0;
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            free(rec->table);
            free(rec->call_stack);
            if (prev == 0)
            {
                threadHashTbl[index] = rec->next;
            }
            else
            {
                prev->next = rec->next;
            }
            free(rec);
            return;
        }
        prev = rec;
        rec = rec->next;
    }
}

void* get_exception_table_addr(thread_t tid)
{
    int index = thread_hash(tid);
    ThreadTblRec* rec = threadHashTbl[index];
    while (rec)
    {
        if (rec->tid == tid)
        {
            return rec->table;
        }
        rec = rec->next;
    }
    return 0;
}
