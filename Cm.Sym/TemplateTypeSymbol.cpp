/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateTypeSymbol.hpp>

namespace Cm { namespace Sym {

TemplateTypeSymbol::TemplateTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_)
{
}

void TemplateTypeSymbol::SetSubjectType(TypeSymbol* subjectType_)
{
    subjectType.reset(subjectType_);
}

void TemplateTypeSymbol::AddTypeArgument(TypeSymbol* typeArgument)
{
    typeArguments.push_back(std::unique_ptr<TypeSymbol>(typeArgument));
}

} } // namespace Cm::Sym
