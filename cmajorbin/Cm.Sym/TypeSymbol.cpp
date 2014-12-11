/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Writer.hpp>

namespace Cm { namespace Sym {

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_, const TypeId& id_) : ContainerSymbol(span_, name_), id(id_)
{
}

TypeSymbol* TypeSymbol::GetBaseType(const SymbolTable& symbolTable) const
{
    TypeSymbol* baseType = symbolTable.GetType(Id().BaseTypeId());
    return baseType;
}

void TypeSymbol::Write(Writer& writer)
{
    ContainerSymbol::Write(writer);
    writer.Write(id);
}

void TypeSymbol::Read(Reader& reader)
{
//  todo
}

} } // namespace Cm::Sym