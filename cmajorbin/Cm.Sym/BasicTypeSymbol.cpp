/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Sym {

Cm::Util::Uuid basicTypeIds[uint8_t(ShortBasicTypeId::max)] =
{
    uint8_t(ShortBasicTypeId::boolId), uint8_t(ShortBasicTypeId::charId), uint8_t(ShortBasicTypeId::voidId), 
    uint8_t(ShortBasicTypeId::sbyteId), uint8_t(ShortBasicTypeId::byteId), uint8_t(ShortBasicTypeId::shortId), uint8_t(ShortBasicTypeId::ushortId), 
    uint8_t(ShortBasicTypeId::intId), uint8_t(ShortBasicTypeId::uintId), uint8_t(ShortBasicTypeId::longId), uint8_t(ShortBasicTypeId::ulongId), 
    uint8_t(ShortBasicTypeId::floatId), uint8_t(ShortBasicTypeId::doubleId)
};

const Cm::Util::Uuid& GetBasicTypeId(ShortBasicTypeId shortId)
{
    return basicTypeIds[uint8_t(shortId)];
}

BasicTypeSymbol::BasicTypeSymbol(const std::string& name_, ShortBasicTypeId id_) : TypeSymbol(Span(), name_, GetBasicTypeId(id_))
{
    SetBound();
    SetSource(SymbolSource::library);
}

BoolTypeSymbol::BoolTypeSymbol() : BasicTypeSymbol("bool", ShortBasicTypeId::boolId)
{
}

CharTypeSymbol::CharTypeSymbol() : BasicTypeSymbol("char", ShortBasicTypeId::charId)
{
}

VoidTypeSymbol::VoidTypeSymbol() : BasicTypeSymbol("void", ShortBasicTypeId::voidId)
{
}

SByteTypeSymbol::SByteTypeSymbol() : BasicTypeSymbol("sbyte", ShortBasicTypeId::sbyteId)
{
}

ByteTypeSymbol::ByteTypeSymbol() : BasicTypeSymbol("byte", ShortBasicTypeId::byteId)
{
}

ShortTypeSymbol::ShortTypeSymbol() : BasicTypeSymbol("short", ShortBasicTypeId::shortId)
{
}

UShortTypeSymbol::UShortTypeSymbol() : BasicTypeSymbol("ushort", ShortBasicTypeId::ushortId)
{
}

IntTypeSymbol::IntTypeSymbol() : BasicTypeSymbol("int", ShortBasicTypeId::intId)
{
}

UIntTypeSymbol::UIntTypeSymbol() : BasicTypeSymbol("uint", ShortBasicTypeId::uintId)
{
}

LongTypeSymbol::LongTypeSymbol() : BasicTypeSymbol("long", ShortBasicTypeId::longId)
{
}

ULongTypeSymbol::ULongTypeSymbol() : BasicTypeSymbol("ulong", ShortBasicTypeId::ulongId)
{
}

FloatTypeSymbol::FloatTypeSymbol() : BasicTypeSymbol("float", ShortBasicTypeId::floatId)
{
}

DoubleTypeSymbol::DoubleTypeSymbol() : BasicTypeSymbol("double", ShortBasicTypeId::doubleId)
{
}

} } // namespace Cm::Sym
