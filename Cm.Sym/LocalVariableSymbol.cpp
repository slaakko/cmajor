/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>

namespace Cm { namespace Sym {

LocalVariableSymbol::LocalVariableSymbol(const Span& span_, const std::string& name_) : VariableSymbol(span_, name_), used(false)
{
}

bool LocalVariableSymbol::IsExportSymbol() const
{
    return Source() == SymbolSource::project && Serialize();
}

void LocalVariableSymbol::SetUseSpan(const Cm::Parsing::Span& useSpan_)
{
    useSpan = useSpan_;
}

void LocalVariableSymbol::ReplaceReplicaTypes()
{
    TypeSymbol* type = GetType();
    if (type->IsReplica() && type->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(type);
        type = replica->GetPrimaryTemplateTypeSymbol();
    }
}

} } // namespace Cm::Sym
