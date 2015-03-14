/*========================================================================
	Copyright (c) 2012-2015 Seppo Laakko
	http://sourceforge.net/projects/cmajor/

	Distributed under the GNU General Public License, version 3 (GPLv3).
	(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_MUTEX_TABLE_INCLUDED
#define CM_SYM_MUTEX_TABLE_INCLUDED

namespace Cm { namespace Sym {

class MutexTable
{
public:
	MutexTable();
	void AddLibraryMutexes(int numLibraryMutexes);
	int GetNextMutexId();
	int GetNumberOfMutexes() const;
private:
	int nextMutexId;
};

MutexTable* GetMutexTable();
void SetMutexTable(MutexTable* mutexTable);

} } // namespace Cm::Sym

#endif // CM_SYM_MUTEX_TABLE_INCLUDED
