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
void dbgheap_watch(int serial);
void dbgheap_report();
void* dbgheap_malloc(unsigned long long size);
void dbgheap_free(void* mem);
void dbgheap_print_info(void* mem);
void dbgheap_enable_free();
void dbgheap_disable_free();

#endif
