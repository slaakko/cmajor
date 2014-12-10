/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Sym {

BasicTypeSymbol::BasicTypeSymbol(const std::string& name_, BasicTypeId id_) : TypeSymbol(Span(), name_, Cm::Util::Uuid(static_cast<uint8_t>(id_)))
{
}

BoolTypeSymbol::BoolTypeSymbol() : BasicTypeSymbol("bool", BasicTypeId::boolId)
{
}

SByteTypeSymbol::SByteTypeSymbol() : BasicTypeSymbol("sbyte", BasicTypeId::sbyteId)
{
}

ByteTypeSymbol::ByteTypeSymbol() : BasicTypeSymbol("byte", BasicTypeId::byteId)
{
}

ShortTypeSymbol::ShortTypeSymbol() : BasicTypeSymbol("short", BasicTypeId::shortId)
{
}

UShortTypeSymbol::UShortTypeSymbol() : BasicTypeSymbol("ushort", BasicTypeId::ushortId)
{
}

IntTypeSymbol::IntTypeSymbol() : BasicTypeSymbol("int", BasicTypeId::intId)
{
}

UIntTypeSymbol::UIntTypeSymbol() : BasicTypeSymbol("uint", BasicTypeId::uintId)
{
}

LongTypeSymbol::LongTypeSymbol() : BasicTypeSymbol("long", BasicTypeId::longId)
{
}

ULongTypeSymbol::ULongTypeSymbol() : BasicTypeSymbol("ulong", BasicTypeId::ulongId)
{
}

FloatTypeSymbol::FloatTypeSymbol() : BasicTypeSymbol("float", BasicTypeId::floatId)
{
}

DoubleTypeSymbol::DoubleTypeSymbol() : BasicTypeSymbol("double", BasicTypeId::doubleId)
{
}

CharTypeSymbol::CharTypeSymbol() : BasicTypeSymbol("char", BasicTypeId::charId)
{
}

VoidTypeSymbol::VoidTypeSymbol() : BasicTypeSymbol("void", BasicTypeId::voidId)
{
}

} } // namespace Cm::Sym
