/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_BASIC_TYPE_SYMBOL_INCLUDED
#define CM_SYM_BASIC_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

enum class ShortBasicTypeId : uint8_t
{
    boolId, charId, voidId, sbyteId, byteId, shortId, ushortId, intId, uintId, longId, ulongId, floatId, doubleId, max
};

const Cm::Util::Uuid& GetBasicTypeId(ShortBasicTypeId shortId);

class BasicTypeSymbol : public TypeSymbol
{
public:
    BasicTypeSymbol(const std::string& name_, ShortBasicTypeId id_);
    bool IsBasicTypeSymbol() const override { return true; }
    std::string TypeString() const override { return "basic type"; };
};

class BoolTypeSymbol : public BasicTypeSymbol
{
public:
    BoolTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::boolSymbol; }
    bool IsBoolTypeSymbol() const override { return true; }
};

class CharTypeSymbol : public BasicTypeSymbol
{
public:
    CharTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::charSymbol; }
    bool IsCharTypeSymbol() const override { return true; }
};

class VoidTypeSymbol : public BasicTypeSymbol
{
public:
    VoidTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::voidSymbol; }
};

class SByteTypeSymbol : public BasicTypeSymbol
{
public:
    SByteTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::sbyteSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class ByteTypeSymbol : public BasicTypeSymbol
{
public:
    ByteTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::byteSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class ShortTypeSymbol : public BasicTypeSymbol
{
public:
    ShortTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::shortSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class UShortTypeSymbol : public BasicTypeSymbol
{
public:
    UShortTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::ushortSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class IntTypeSymbol : public BasicTypeSymbol
{
public:
    IntTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::intSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class UIntTypeSymbol : public BasicTypeSymbol
{
public:
    UIntTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::uintSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class LongTypeSymbol : public BasicTypeSymbol
{
public:
    LongTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::longSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class ULongTypeSymbol : public BasicTypeSymbol
{
public:
    ULongTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::ulongSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
};

class FloatTypeSymbol : public BasicTypeSymbol
{
public:
    FloatTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::floatSymbol; }
    bool IsFloatingPointTypeSymbol() const override { return true; }
};

class DoubleTypeSymbol : public BasicTypeSymbol
{
public:
    DoubleTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::doubleSymbol; }
    bool IsFloatingPointTypeSymbol() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_BASIC_TYPE_SYMBOL_INCLUDED
