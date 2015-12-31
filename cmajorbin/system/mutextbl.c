/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <pthread.h>
#include <stdlib.h>

static int nmtx = 0;
static pthread_mutex_t* mutex_tab = 0;

int init_recursive_mutex(pthread_mutex_t* mtx);
int destroy_mutex(pthread_mutex_t* mtx);
int lock_mutex(pthread_mutex_t* mtx);
int unlock_mutex(pthread_mutex_t* mtx);

#define EXIT_MUTEX_OP_FAILED 248

void mutextbl_init(int numMutexes)
{
    int i;
    
    nmtx = numMutexes;
    mutex_tab = (pthread_mutex_t*)malloc(nmtx * sizeof(pthread_mutex_t));
    for (i = 0; i < nmtx; ++i)
    {
        int result = init_recursive_mutex(&mutex_tab[i]);
        if (result != 0)
        {
            exit(EXIT_MUTEX_OP_FAILED);
        }
    }
}

void mutextbl_done()
{
    int i;
    
    for (i = 0; i < nmtx; ++i)
    {
        int result = destroy_mutex(&mutex_tab[i]);
        if (result != 0)
        {
            exit(EXIT_MUTEX_OP_FAILED);
        }    
    }
    free(mutex_tab);
    mutex_tab = 0;
}

int mutextbl_lock_mtx(int mid)
{
    if (mid >= 0 && mid < nmtx)
    {
        return lock_mutex(&mutex_tab[mid]);
    }    
    return -1;
}

int mutextbl_unlock_mtx(int mid)
{
    if (mid >= 0 && mid < nmtx)
    {
        return unlock_mutex(&mutex_tab[mid]);
    }
    return -1;
}
