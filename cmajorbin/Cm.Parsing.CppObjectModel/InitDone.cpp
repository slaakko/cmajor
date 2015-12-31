/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing.CppObjectModel/Operator.hpp>

namespace Cm { namespace Parsing { namespace CppObjectModel {

void Init()
{
    OperatorInit();
}

void Done()
{
    OperatorDone();
}

} } } // namespace Cm::Parsing::CppObjectModel
