#ifndef THREADTBL_H
#define THREADTBL_H

typedef void* thread_t;
void threadtbl_init(int numExceptions);
void threadtbl_done();
void allocate_thread_data(thread_t tid);
void free_thread_data(thread_t tid);
void* get_exception_table_addr(thread_t tid);
int get_current_exception_id();
void* get_current_exception_addr();
void set_current_exception_id(int exception_id);
void set_current_exception_addr(void* exception_addr);
void reset_current_exception();

#endif // THREADTBL_H
