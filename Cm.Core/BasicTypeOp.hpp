/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_BASIC_TYPE_OP_INCLUDED
#define CM_CORE_BASIC_TYPE_OP_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Core {

class DefaultCtor : public Cm::Sym::FunctionSymbol
{
public:
    DefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class CopyCtor : public Cm::Sym::FunctionSymbol
{
public:
    CopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class CopyAssignment : public Cm::Sym::FunctionSymbol
{
public:
    CopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class MoveCtor : public Cm::Sym::FunctionSymbol
{
public:
    MoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class MoveAssignment : public Cm::Sym::FunctionSymbol
{
public:
    MoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpEqual : public Cm::Sym::FunctionSymbol
{
public:
    OpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpLess : public Cm::Sym::FunctionSymbol
{
public:
    OpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpAdd : public Cm::Sym::FunctionSymbol
{
public:
    OpAdd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpSub : public Cm::Sym::FunctionSymbol
{
public:
    OpSub(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpMul : public Cm::Sym::FunctionSymbol
{
public:
    OpMul(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpDiv : public Cm::Sym::FunctionSymbol
{
public:
    OpDiv(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpRem : public Cm::Sym::FunctionSymbol
{
public:
    OpRem(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpShl : public Cm::Sym::FunctionSymbol
{
public:
    OpShl(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpShr : public Cm::Sym::FunctionSymbol
{
public:
    OpShr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpBitAnd : public Cm::Sym::FunctionSymbol
{
public:
    OpBitAnd(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpBitOr : public Cm::Sym::FunctionSymbol
{
public:
    OpBitOr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpBitXor : public Cm::Sym::FunctionSymbol
{
public:
    OpBitXor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpNot : public Cm::Sym::FunctionSymbol
{
public:
    OpNot(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpUnaryPlus : public Cm::Sym::FunctionSymbol
{
public:
    OpUnaryPlus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpUnaryMinus : public Cm::Sym::FunctionSymbol
{
public:
    OpUnaryMinus(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpComplement : public Cm::Sym::FunctionSymbol
{
public:
    OpComplement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpIncrement : public Cm::Sym::FunctionSymbol
{
public:
    OpIncrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

class OpDecrement : public Cm::Sym::FunctionSymbol
{
public:
    OpDecrement(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type_);
private:
    Cm::Sym::TypeSymbol* type;
};

enum class ConversionType
{
    explicit_, implicit
};

enum class ConversionInst
{
    none, sext, zext, trunc, bitcast, uitofp, sitofp, fptoui, fptosi, fpext, fptrunc, ptrtoint
};

enum class ConversionRank
{
    exactMatch, promotion, conversion
};

inline bool BetterConversionRank(ConversionRank left, ConversionRank right)
{
    return left < right;
}

class ConvertingCtor : public Cm::Sym::FunctionSymbol
{
public:
    ConvertingCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* targetType_, Cm::Sym::TypeSymbol* sourceType_, ConversionType conversionType_, ConversionInst conversionInst_, 
        ConversionRank conversionRank_, int conversionDistance_);
    ConversionType GetConversionType() const { return conversionType; }
    ConversionRank GetConversionRank() const { return conversionRank; }
    int GetConversionDistance() const { return conversionDistance; }
    bool IsConvertingConstructor() const override { return true; }
private:
    Cm::Sym::TypeSymbol* targetType;
    Cm::Sym::TypeSymbol* sourceType;
    ConversionType conversionType;
    ConversionInst conversionInst;
    ConversionRank conversionRank;
    int conversionDistance;
};

} } // namespace Cm::Core

#endif // CM_CORE_BASIC_TYPE_OP_INCLUDED
