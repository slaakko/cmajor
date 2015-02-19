/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

DelegateTypeSymbol::DelegateTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), flags(DelegateTypeSymbolFlags::none)
{
}

std::string DelegateTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

bool DelegateTypeSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return TypeSymbol::IsExportSymbol();
}

ClassDelegateTypeSymbol::ClassDelegateTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), flags(ClassDelegateTypeSymbolFlags::none)
{
}

std::string ClassDelegateTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

bool ClassDelegateTypeSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    return TypeSymbol::IsExportSymbol();
}

} } // namespace Cm::Sym