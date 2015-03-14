/*========================================================================
	Copyright (c) 2012-2015 Seppo Laakko
	http://sourceforge.net/projects/cmajor/

	Distributed under the GNU General Public License, version 3 (GPLv3).
	(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/MutexTable.hpp>

namespace Cm { namespace Sym {

MutexTable::MutexTable() : nextMutexId(0)
{
}

void MutexTable::AddLibraryMutexes(int numLibraryMutexes) 
{ 
	nextMutexId += numLibraryMutexes; 
}

int MutexTable::GetNextMutexId() 
{ 
	return nextMutexId++; 
}

int MutexTable::GetNumberOfMutexes() const
{ 
	return nextMutexId; 
}

MutexTable* globalMutexTable = nullptr;

MutexTable* GetMutexTable()
{
	return globalMutexTable;
}

void SetMutexTable(MutexTable* mutexTable)
{
	globalMutexTable = mutexTable;
}

} } // namespace Cm::Sym
