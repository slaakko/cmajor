/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

Cm::Ast::DerivationList emptyDerivationList;

bool operator<(const DerivationCounts& left, const DerivationCounts& right)
{
    if (left.consts < right.consts)
    {
        return true;
    }
    else if (right.consts < left.consts)
    {
        return false;
    }
    else if (left.refs < right.refs)
    {
        return true;
    }
    else if (right.refs < left.refs)
    {
        return false;
    }
    else if (left.rvalueRefs < right.rvalueRefs)
    {
        return true;
    }
    else if (right.rvalueRefs < left.rvalueRefs)
    {
        return false;
    }
    else if (left.pointers < right.pointers)
    {
        return true;
    }
    else
    {
        return false;
    }
}

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

TypeSymbol::TypeSymbol(const Span& span_, const std::string& name_, const TypeId& id_) : ContainerSymbol(span_, name_), id(id_)
{
}

void TypeSymbol::Write(Writer& writer)
{
    ContainerSymbol::Write(writer);
    if (Name().find("UniquePtr") != std::string::npos)
    {
        int x = 0;
    }
    writer.Write(id);
}

void TypeSymbol::Read(Reader& reader)
{
    ContainerSymbol::Read(reader);
    if (Name().find("UniquePtr") != std::string::npos)
    {
        int x = 0;
    }
    id = reader.ReadTypeId();
    reader.BackpatchType(this);
}

void TypeSymbol::SetIrType(Ir::Intf::Type* irType_)
{
    if (irType_->Owned())
    {
        throw std::runtime_error("ir type already owned");
    }
    irType_->SetOwned();
    irType.reset(irType_);
}

Ir::Intf::Type* TypeSymbol::GetIrType() const
{
    return irType.get();
}

void TypeSymbol::SetDefaultIrValue(Ir::Intf::Object* defaultIrValue_)
{
    if (defaultIrValue_->Owned())
    {
        throw std::runtime_error("default ir value already owned");
    }
    defaultIrValue_->SetOwned();
    defaultIrValue.reset(defaultIrValue_);
}

Ir::Intf::Object* TypeSymbol::GetDefaultIrValue() const
{
    return defaultIrValue.get();
}

} } // namespace Cm::Sym