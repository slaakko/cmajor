/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing.CppObjectModel/Literal.hpp>
#include <Cm.Parsing.CppObjectModel/Visitor.hpp>

namespace Cm { namespace Parsing { namespace CppObjectModel {

Literal::Literal(const std::string& name_): CppObject(name_)
{
}

void Literal::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } } // namespace Cm::Parsing::CppObjectModel
