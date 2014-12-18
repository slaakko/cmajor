/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_TEMPORARY_INCLUDED
#define LLVM_IR_TEMPORARY_INCLUDED

namespace Llvm { 

int GetNextTemporaryNumber();
void ResetTemporaryCounter();

} // namespace Llvm

#endif // LLVM_IR_TEMPORARY_INCLUDED
