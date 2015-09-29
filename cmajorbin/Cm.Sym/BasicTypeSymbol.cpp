/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

Cm::Util::Uuid basicTypeIds[uint8_t(ShortBasicTypeId::max)] =
{
    uint8_t(0), uint8_t(ShortBasicTypeId::boolId), uint8_t(ShortBasicTypeId::charId), uint8_t(ShortBasicTypeId::voidId),
    uint8_t(ShortBasicTypeId::sbyteId), uint8_t(ShortBasicTypeId::byteId), uint8_t(ShortBasicTypeId::shortId), uint8_t(ShortBasicTypeId::ushortId), 
    uint8_t(ShortBasicTypeId::intId), uint8_t(ShortBasicTypeId::uintId), uint8_t(ShortBasicTypeId::longId), uint8_t(ShortBasicTypeId::ulongId), 
    uint8_t(ShortBasicTypeId::floatId), uint8_t(ShortBasicTypeId::doubleId), uint8_t(ShortBasicTypeId::nullPtrId)
};

const Cm::Util::Uuid& GetBasicTypeId(ShortBasicTypeId shortId)
{
    return basicTypeIds[uint8_t(shortId)];
}

BasicTypeSymbol::BasicTypeSymbol(const std::string& name_, ShortBasicTypeId id_) : TypeSymbol(Span(), name_, GetBasicTypeId(id_))
{
    SetBound();
    SetSource(SymbolSource::library);
    SetPublic();
}

void BasicTypeSymbol::Dump(CodeFormatter& formatter)
{
}

BoolTypeSymbol::BoolTypeSymbol() : BasicTypeSymbol("bool", ShortBasicTypeId::boolId)
{
    SetIrType(Cm::IrIntf::I1());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

CharTypeSymbol::CharTypeSymbol() : BasicTypeSymbol("char", ShortBasicTypeId::charId)
{
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        SetIrType(Cm::IrIntf::I8());
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        SetIrType(Cm::IrIntf::Char());
    }
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

VoidTypeSymbol::VoidTypeSymbol() : BasicTypeSymbol("void", ShortBasicTypeId::voidId)
{
    SetIrType(Cm::IrIntf::Void());
}

SByteTypeSymbol::SByteTypeSymbol() : BasicTypeSymbol("sbyte", ShortBasicTypeId::sbyteId)
{
    SetIrType(Cm::IrIntf::I8());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

ByteTypeSymbol::ByteTypeSymbol() : BasicTypeSymbol("byte", ShortBasicTypeId::byteId)
{
    SetIrType(Cm::IrIntf::UI8());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

ShortTypeSymbol::ShortTypeSymbol() : BasicTypeSymbol("short", ShortBasicTypeId::shortId)
{
    SetIrType(Cm::IrIntf::I16());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

UShortTypeSymbol::UShortTypeSymbol() : BasicTypeSymbol("ushort", ShortBasicTypeId::ushortId)
{
    SetIrType(Cm::IrIntf::UI16());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

IntTypeSymbol::IntTypeSymbol() : BasicTypeSymbol("int", ShortBasicTypeId::intId)
{
    SetIrType(Cm::IrIntf::I32());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

UIntTypeSymbol::UIntTypeSymbol() : BasicTypeSymbol("uint", ShortBasicTypeId::uintId)
{
    SetIrType(Cm::IrIntf::UI32());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

LongTypeSymbol::LongTypeSymbol() : BasicTypeSymbol("long", ShortBasicTypeId::longId)
{
    SetIrType(Cm::IrIntf::I64());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

ULongTypeSymbol::ULongTypeSymbol() : BasicTypeSymbol("ulong", ShortBasicTypeId::ulongId)
{
    SetIrType(Cm::IrIntf::UI64());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

FloatTypeSymbol::FloatTypeSymbol() : BasicTypeSymbol("float", ShortBasicTypeId::floatId)
{
    SetIrType(Cm::IrIntf::Float());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

DoubleTypeSymbol::DoubleTypeSymbol() : BasicTypeSymbol("double", ShortBasicTypeId::doubleId)
{
    SetIrType(Cm::IrIntf::Double());
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

NullPtrTypeSymbol::NullPtrTypeSymbol() : BasicTypeSymbol("@nullptrtype", ShortBasicTypeId::nullPtrId)
{
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
    {
        SetIrType(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1));
    }
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        SetIrType(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1));
    }
    SetDefaultIrValue(GetIrType()->CreateDefaultValue());
}

} } // namespace Cm::Sym
