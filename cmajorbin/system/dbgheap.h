/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef DBGHEAP_INCLUDED
#define DBGHEAP_INCLUDED
#include <stdlib.h>

void dbgheap_init();
void dbgheap_done();
void dbgheap_stop(int allocation);
void dbgheap_report();
void* dbgheap_malloc(unsigned long long size, const char* file, int line);
void dbgheap_free(void* mem);
int dbgheap_get_serial(void* mem);

#endif
