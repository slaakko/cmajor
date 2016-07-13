/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Meta.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Exception.hpp>

namespace Cm { namespace Sym {

IntrinsicFunction::~IntrinsicFunction()
{
}

void IntrinsicFunction::Evaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments)
{
    if (stack.Count() != Arity())
    {
        throw Exception("wrong number of parameters for intrinsic " + std::string(GroupName()), Span());
    }
    if (int(templateArguments.size()) != NumberOfTypeParameters())
    {
        throw Exception("wrong number of template arguments for intrinsic " + std::string(GroupName()), Span());
    }
    DoEvaluate(stack, templateArguments);
}

FunctionSymbol* CreateIntrinsic(IntrinsicFunction* intrinsic)
{
    FunctionSymbol* fun = new FunctionSymbol(Span(), intrinsic->GroupName());
    fun->SetGroupName(intrinsic->GroupName());
    fun->SetIntrinsic();
    fun->SetIntrinsic(intrinsic);
    int n = intrinsic->NumberOfTypeParameters();
    for (int i = 0; i < n; ++i)
    {
        std::string p = "T" + std::to_string(i);
        fun->AddSymbol(new Cm::Sym::TypeParameterSymbol(Span(), p));
    }
    fun->ComputeName();
    return fun;
}

class TypePredicate : public IntrinsicFunction
{
public:
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
};

class IsLvalueReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsLvalueReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsReferenceType()));
    }
};

class IsNonLvalueReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonLvalueReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsReferenceType()));
    }
};

class IsRvalueReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsRvalueReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsRvalueRefType()));
    }
};

class IsNonRvalueReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonRvalueReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsRvalueRefType()));
    }
};

class IsReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsReferenceType() || type->IsRvalueRefType()));
    }
};

class IsNonReferenceTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonReferenceType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!(type->IsReferenceType() || type->IsRvalueRefType())));
    }
};

class IsConstTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsConstType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsConstType()));
    }
};

class IsNonConstTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonConstType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsConstType()));
    }
};

class IsPointerTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsPointerType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsPointerType()));
    }
};

class IsNonPointerTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonPointerType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsPointerType()));
    }
};

class IsIntegerTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsIntegerType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsIntegerTypeSymbol()));
    }
};

class IsNonIntegerTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonIntegerType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsIntegerTypeSymbol()));
    }
};

class IsFloatingPointTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsFloatingPointType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsFloatingPointTypeSymbol()));
    }
};

class IsNonFloatingPointTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonFloatingPointType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsFloatingPointTypeSymbol()));
    }
};

class IsSignedTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsSignedType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsSignedType()));
    }
};

class IsNonSignedTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonSignedType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsSignedType()));
    }
};

class IsUnsignedTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsUnsignedType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsUnsignedType()));
    }
};

class IsNonUnsignedTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonUnsignedType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsUnsignedType()));
    }
};

class IsBasicTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsBasicType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsBasicTypeSymbol()));
    }
};

class IsNonBasicTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonBasicType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsBasicTypeSymbol()));
    }
};

class IsCharacterTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsCharacterType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsCharacterTypeSymbol()));
    }
};

class IsNonCharacterTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonCharacterType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsCharacterTypeSymbol()));
    }
};

class IsClassTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsClassType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsClassTypeSymbol()));
    }
};

class IsNonClassTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonClassType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsClassTypeSymbol()));
    }
};

class IsPolymorphicPredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsPolymorphic"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsPolymorphic()));
    }
};

class IsNonPolymorphicPredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonPolymorphic"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsPolymorphic()));
    }
};

class IsDelegateTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsDelegateType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsDelegateTypeSymbol()));
    }
};

class IsNonDelegateTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonDelegateType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsDelegateTypeSymbol()));
    }
};

class IsClassDelegateTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsClassDelegateType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsClassDelegateTypeSymbol()));
    }
};

class IsNonClassDelegateTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonClassDelegateType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsClassDelegateTypeSymbol()));
    }
};

class IsEnumTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsEnumType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsEnumTypeSymbol()));
    }
};

class IsNonEnumTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonEnumType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsEnumTypeSymbol()));
    }
};

class IsBoolTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsBoolType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsBoolTypeSymbol()));
    }
};

class IsNonBoolTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonBoolType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsBoolTypeSymbol()));
    }
};

class IsCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsCharTypeSymbol()));
    }
};

class IsNonCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsCharTypeSymbol()));
    }
};

class IsWCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsWCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsWCharTypeSymbol()));
    }
};

class IsNonWCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonWCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsWCharTypeSymbol()));
    }
};

class IsUCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsUCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsUCharTypeSymbol()));
    }
};

class IsNonUCharTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonUCharType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsUCharTypeSymbol()));
    }
};

class IsVoidTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsVoidType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsVoidTypeSymbol()));
    }
};

class IsNonVoidTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonVoidType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsVoidTypeSymbol()));
    }
};

class IsSByteTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsSByteType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsSByteTypeSymbol()));
    }
};

class IsNonSByteTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonSByteType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsSByteTypeSymbol()));
    }
};

class IsByteTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsByteType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsByteTypeSymbol()));
    }
};

class IsNonByteTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonByteType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsByteTypeSymbol()));
    }
};

class IsShortTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsShortType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsShortTypeSymbol()));
    }
};

class IsNonShortTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonShortType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsShortTypeSymbol()));
    }
};

class IsUShortTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsUShortType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsUShortTypeSymbol()));
    }
};

class IsNonUShortTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonUShortType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsUShortTypeSymbol()));
    }
};

class IsIntTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsIntType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsIntTypeSymbol()));
    }
};

class IsNonIntTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonIntType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsIntTypeSymbol()));
    }
};

class IsUIntTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsUIntType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsUIntTypeSymbol()));
    }
};

class IsNonUIntTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonUIntType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsUIntTypeSymbol()));
    }
};

class IsLongTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsLongType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsLongTypeSymbol()));
    }
};

class IsNonLongTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonLongType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsLongTypeSymbol()));
    }
};

class IsULongTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsULongType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsULongTypeSymbol()));
    }
};

class IsNonULongTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonULongType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsULongTypeSymbol()));
    }
};

class IsFloatTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsFloatType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsFloatTypeSymbol()));
    }
};

class IsNonFloatTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonFloatType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsFloatTypeSymbol()));
    }
};

class IsDoubleTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsDoubleType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(type->IsDoubleTypeSymbol()));
    }
};

class IsNonDoubleTypePredicate : public TypePredicate
{
public:
    const char* GroupName() const override { return "IsNonDoubleType"; }
    void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) override
    {
        Cm::Sym::TypeSymbol* type = templateArguments.front();
        stack.Push(new BoolValue(!type->IsDoubleTypeSymbol()));
    }
};

void MetaInit(SymbolTable& symbolTable)
{
    symbolTable.BeginNamespaceScope("System.Meta", Span());
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsLvalueReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonLvalueReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsRvalueReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonRvalueReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonReferenceTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsConstTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonConstTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsIntegerTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonIntegerTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsFloatingPointTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonFloatingPointTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsSignedTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonSignedTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsUnsignedTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonUnsignedTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsBasicTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonBasicTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsCharacterTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonCharacterTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsPointerTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonPointerTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsClassTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonClassTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsPolymorphicPredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonPolymorphicPredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsDelegateTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonDelegateTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsClassDelegateTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonClassDelegateTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsEnumTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonEnumTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsBoolTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonBoolTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsWCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonWCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsUCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonUCharTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsSByteTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonSByteTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsByteTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonByteTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsShortTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonShortTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsUShortTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonUShortTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsIntTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonIntTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsUIntTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonUIntTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsLongTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonLongTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsULongTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonULongTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsFloatTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonFloatTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsDoubleTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonDoubleTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsVoidTypePredicate()));
    symbolTable.Container()->AddSymbol(CreateIntrinsic(new IsNonVoidTypePredicate()));
    symbolTable.EndNamespaceScope();
}

} } // namespace Cm::Sym