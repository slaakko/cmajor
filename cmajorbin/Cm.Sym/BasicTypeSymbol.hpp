/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
    boolId = 1, charId, wcharId, ucharId, voidId, sbyteId, byteId, shortId, ushortId, intId, uintId, longId, ulongId, floatId, doubleId, nullPtrId, max
};

const TypeId& GetBasicTypeId(ShortBasicTypeId shortId);

class BasicTypeSymbol : public TypeSymbol
{
public:
    BasicTypeSymbol(const std::string& name_, ShortBasicTypeId id_);
    bool IsBasicTypeSymbol() const override { return true; }
    std::string TypeString() const override { return "basic type"; };
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsValueTypeSymbol() const override { return true; }
    void Dump(CodeFormatter& formatter) override;
};

class BoolTypeSymbol : public BasicTypeSymbol
{
public:
    BoolTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::boolSymbol; }
    bool IsBoolTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "bo"; }
};

class CharTypeSymbol : public BasicTypeSymbol
{
public:
    CharTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::charSymbol; }
    bool IsCharTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "ch"; }
};

class WCharTypeSymbol : public BasicTypeSymbol
{
public:
    WCharTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::wcharSymbol; }
    bool IsWCharTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "wc"; }
};

class UCharTypeSymbol : public BasicTypeSymbol
{
public:
    UCharTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::ucharSymbol; }
    bool IsWCharTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "uc"; }
};

class VoidTypeSymbol : public BasicTypeSymbol
{
public:
    VoidTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::voidSymbol; }
    std::string GetMangleId() const override { return "vo"; }
    bool IsVoidTypeSymbol() const override { return true; }
};

class SByteTypeSymbol : public BasicTypeSymbol
{
public:
    SByteTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::sbyteSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    bool IsSignedType() const override { return true; }
    std::string GetMangleId() const override { return "sb"; }
};

class ByteTypeSymbol : public BasicTypeSymbol
{
public:
    ByteTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::byteSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "by"; }
};

class ShortTypeSymbol : public BasicTypeSymbol
{
public:
    ShortTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::shortSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    bool IsSignedType() const override { return true; }
    std::string GetMangleId() const override { return "sh"; }
};

class UShortTypeSymbol : public BasicTypeSymbol
{
public:
    UShortTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::ushortSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "us"; }
};

class IntTypeSymbol : public BasicTypeSymbol
{
public:
    IntTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::intSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    bool IsSignedType() const override { return true; }
    std::string GetMangleId() const override { return "in"; }
};

class UIntTypeSymbol : public BasicTypeSymbol
{
public:
    UIntTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::uintSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "ui"; }
};

class LongTypeSymbol : public BasicTypeSymbol
{
public:
    LongTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::longSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    bool IsSignedType() const override { return true; }
    std::string GetMangleId() const override { return "lo"; }
};

class ULongTypeSymbol : public BasicTypeSymbol
{
public:
    ULongTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::ulongSymbol; }
    bool IsIntegerTypeSymbol() const override { return true; }
    bool IsUlongType() const override { return true; }
    std::string GetMangleId() const override { return "ul"; }
};

class FloatTypeSymbol : public BasicTypeSymbol
{
public:
    FloatTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::floatSymbol; }
    bool IsFloatingPointTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "fl"; }
};

class DoubleTypeSymbol : public BasicTypeSymbol
{
public:
    DoubleTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::doubleSymbol; }
    bool IsFloatingPointTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return "do"; }
};

class NullPtrTypeSymbol : public BasicTypeSymbol
{
public:
    NullPtrTypeSymbol();
    SymbolType GetSymbolType() const override { return SymbolType::nullptrSymbol; }
    std::string GetMangleId() const override { return "np"; }
    bool IsNullPtrType() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_BASIC_TYPE_SYMBOL_INCLUDED
