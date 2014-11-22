/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing.CppObjectModel/InitDone.hpp>

namespace Cm { namespace Parsing {

void Init()
{
    Cm::Parsing::CppObjectModel::Init();
    ParsingDomainInit();
    KeywordInit();
}

void Done()
{
    KeywordDone();
    ParsingDomainDone();
    Cm::Parsing::CppObjectModel::Done();
}

} } // namespace Cm::Parsing

