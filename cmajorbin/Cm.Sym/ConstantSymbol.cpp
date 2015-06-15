/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

ConstantSymbol::ConstantSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr), evaluating(false)
{
}

bool ConstantSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    return Symbol::IsExportSymbol();
}

void ConstantSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    writer.Write(type->Id());
    writer.Write(value.get());
}

void ConstantSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    reader.FetchTypeFor(this, 0);
    value.reset(reader.ReadValue());
}

void ConstantSymbol::SetType(TypeSymbol* type_, int index)
{
    type = type_;
}

void ConstantSymbol::SetValue(Value* value_)
{
    value.reset(value_);
}

void ConstantSymbol::Dump(CodeFormatter& formatter)
{
    if (!IsProject()) return;
    std::string valueStr;
    if (value)
    {
        valueStr.append(" = ").append(value->ToString());
    }
    formatter.WriteLine(SymbolFlagStr(Flags(), DeclaredAccess(), true) + " " + TypeString() + " " + Name() + valueStr);
}

} } // namespace Cm::Sym
