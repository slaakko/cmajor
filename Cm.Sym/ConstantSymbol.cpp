/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

ConstantSymbol::ConstantSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr), evaluating(false)
{
}

void ConstantSymbol::Write(Writer& writer)
{
    writer.Write(type->Id());
    value->Write(writer.GetBinaryWriter());
}

void ConstantSymbol::Read(Reader& reader)
{
    // todo
}

void ConstantSymbol::SetType(TypeSymbol* type_)
{
    type = type_;
}

void ConstantSymbol::SetValue(Value* value_)
{
    value.reset(value_);
}

} } // namespace Cm::Sym
