/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
#define CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

class TemplateTypeSymbol : public TypeSymbol
{
public:
    TemplateTypeSymbol(const Span& span_, const std::string& name_);
    void SetSubjectType(TypeSymbol* subjectType_);
    void AddTypeArgument(TypeSymbol* typeArgument);
private:
    std::unique_ptr<TypeSymbol> subjectType;
    std::vector<std::unique_ptr<TypeSymbol>> typeArguments;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
