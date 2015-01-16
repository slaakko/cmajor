/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.Bind/Parameter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>

namespace Cm { namespace Bind {

Cm::BoundTree::BoundInitClassObjectStatement* GenerateBaseConstructorCall(const Cm::Parsing::Span& span, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ClassTypeSymbol* baseClassType, Cm::Sym::ParameterSymbol* thisParam, Cm::BoundTree::BoundExpressionList& arguments, const std::string& errorMessageHeader, Cm::Core::Exception*& exception)
{
    compileUnit.IrClassTypeRepository().AddClassType(baseClassType);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* baseClassPtrType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClassType, span);
    Cm::Core::Argument baseClassArg(Cm::Core::ArgumentCategory::lvalue, baseClassPtrType);
    resolutionArguments.push_back(baseClassArg);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, baseClassType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* baseClassCtor = nullptr;
    try
    {
        baseClassCtor = ResolveOverload(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), 
            compileUnit.SynthesizedClassFunRepository(), "@constructor", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception = new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because base class constructor not found: " + ex.Message(), ex.Defined(), ex.Referenced());
        return nullptr;
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    arguments.InsertFront(thisAsBase); // insert 'this' to front
    PrepareFunctionArguments(baseClassCtor, arguments, false, compileUnit.IrClassTypeRepository());
    int n = int(conversions.size());
    if (n != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
            Cm::BoundTree::BoundExpression* arg = argument.release();
            argument.reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
    functionCall->SetFunction(baseClassCtor);
    Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    return initBaseClasObjectStatement;
}

Cm::BoundTree::BoundInitMemberVariableStatement* GenerateInitMemberVariableStatement(const Cm::Parsing::Span& span, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ParameterSymbol* thisParam, Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments, const std::string& errorMessageHeader, Cm::Core::Exception*& exception)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, compileUnit.SymbolTable().GetTypeRepository().MakePointerType(memberVariableType, span));
    resolutionArguments.push_back(variableArgument);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, memberVariableType->GetContainerScope()->ClassOrNsScope()));
    if (memberVariableType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
        compileUnit.IrClassTypeRepository().AddClassType(memberVarClassType);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberCtor = nullptr;
    try
    {
        memberCtor = ResolveOverload(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), 
            compileUnit.SynthesizedClassFunRepository(), "@constructor", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception = new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because member variable constructor for member variable '" + memberVariableSymbol->Name() + 
            "' not found: " + ex.Message(), ex.Defined(), ex.Referenced());
        return nullptr;
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    boundMemberVariable->SetClassObject(boundThisParam);
    arguments.InsertFront(boundMemberVariable);
    PrepareFunctionArguments(memberCtor, arguments, true, compileUnit.IrClassTypeRepository());
    int n = int(conversions.size());
    if (n != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
            Cm::BoundTree::BoundExpression* arg = argument.release();
            argument.reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = new Cm::BoundTree::BoundInitMemberVariableStatement(memberCtor, std::move(arguments));
    return initMemberVariableStatement;
}

Cm::BoundTree::BoundFunctionCallStatement* GenerateBaseAssignmentCall(const Cm::Parsing::Span& span, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::ClassTypeSymbol* classTypeSymbol,
    Cm::Sym::ClassTypeSymbol* baseClassType, Cm::Sym::ParameterSymbol* thisParam, Cm::BoundTree::BoundExpressionList& arguments, const std::string& errorMessageHeader, Cm::Core::Exception*& exception)
{
    compileUnit.IrClassTypeRepository().AddClassType(baseClassType);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* baseClassPtrType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClassType, span);
    Cm::Core::Argument baseClassArg(Cm::Core::ArgumentCategory::lvalue, baseClassPtrType);
    resolutionArguments.push_back(baseClassArg);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, baseClassType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* baseClassAssignment = nullptr;
    try
    {
        baseClassAssignment = ResolveOverload(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(),
            compileUnit.SynthesizedClassFunRepository(), "operator=", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception = new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because base class copy assignment not found: " + ex.Message(), ex.Defined(), ex.Referenced());
        return nullptr;
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    arguments.InsertFront(thisAsBase); // insert 'this' to front
    PrepareFunctionArguments(baseClassAssignment, arguments, false, compileUnit.IrClassTypeRepository());
    int n = int(conversions.size());
    if (n != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
            Cm::BoundTree::BoundExpression* arg = argument.release();
            argument.reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCallStatement* assignBaseClasObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassAssignment, std::move(arguments));
    return assignBaseClasObjectStatement;
}

Cm::BoundTree::BoundFunctionCallStatement* GenerateAssignMemberVariableStatement(const Cm::Parsing::Span& span, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::ClassTypeSymbol* classTypeSymbol,
    Cm::Sym::ParameterSymbol* thisParam, Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments, const std::string& errorMessageHeader, 
    Cm::Core::Exception*& exception)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, compileUnit.SymbolTable().GetTypeRepository().MakePointerType(memberVariableType, span));
    resolutionArguments.push_back(variableArgument);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, memberVariableType->GetContainerScope()->ClassOrNsScope()));
    if (memberVariableType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
        compileUnit.IrClassTypeRepository().AddClassType(memberVarClassType);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberAssignment = nullptr;
    try
    {
        memberAssignment = ResolveOverload(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(),
            compileUnit.SynthesizedClassFunRepository(), "operator=", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception = new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because member variable copy assignment for member variable '" + memberVariableSymbol->Name() +
            "' not found: " + ex.Message(), ex.Defined(), ex.Referenced());
        return nullptr;
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    boundMemberVariable->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    boundMemberVariable->SetClassObject(boundThisParam);
    arguments.InsertFront(boundMemberVariable);
    PrepareFunctionArguments(memberAssignment, arguments, true, compileUnit.IrClassTypeRepository());
    int n = int(conversions.size());
    if (n != arguments.Count())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::FunctionSymbol* conversionFun = conversions[i];
        if (conversionFun)
        {
            std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
            Cm::BoundTree::BoundExpression* arg = argument.release();
            argument.reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            argument->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCallStatement* assignMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberAssignment, std::move(arguments));
    return assignMemberVariableStatement;
}

Cm::Sym::FunctionSymbol* GenerateDefaultConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::FunctionSymbol* defaultConstructorSymbol = new Cm::Sym::FunctionSymbol(span, "@default_ctor");
    defaultConstructorSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    defaultConstructorSymbol->SetGroupName("@constructor");
    defaultConstructorSymbol->SetParent(classTypeSymbol);
    defaultConstructorSymbol->SetConstructorOrDestructorSymbol();
    defaultConstructorSymbol->SetMemberFunctionSymbol();
    if (!unique)
    {
        defaultConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
        defaultConstructorSymbol->SetReplicated();
    }
    defaultConstructorSymbol->AddSymbol(thisParam);
    defaultConstructorSymbol->ComputeName();
    if (!generateImplementation) return defaultConstructorSymbol;
    std::unique_ptr<Cm::BoundTree::BoundFunction> defaultConstructor(new Cm::BoundTree::BoundFunction(nullptr, defaultConstructorSymbol));
    defaultConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), compileUnit.SynthesizedClassFunRepository(),
        defaultConstructor.get());
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = GenerateBaseConstructorCall(span, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments, 
            "cannot generate default constructor", exception);
        if (initBaseClasObjectStatement)
        {
            defaultConstructor->Body()->AddStatement(initBaseClasObjectStatement);
        }
        else
        {
            return nullptr;
        }
    }
    if (classTypeSymbol->IsVirtual())
    {
        defaultConstructor->Body()->AddStatement(new Cm::BoundTree::BoundInitVPtrStatement(classTypeSymbol));
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = GenerateInitMemberVariableStatement(span, compileUnit, classTypeSymbol, thisParam, memberVariableSymbol, arguments,
            "cannot generate default constructor", exception);
        if (initMemberVariableStatement)
        {
            defaultConstructor->Body()->AddStatement(initMemberVariableStatement);
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(defaultConstructor.release());
    return defaultConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateCopyConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::TypeSymbol* constRefType = compileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thatParam = new Cm::Sym::ParameterSymbol(span, "that");
    thatParam->SetType(constRefType);
    Cm::Sym::FunctionSymbol* copyConstructorSymbol = new Cm::Sym::FunctionSymbol(span, "@copy_ctor");
    copyConstructorSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    copyConstructorSymbol->SetGroupName("@constructor");
    copyConstructorSymbol->SetParent(classTypeSymbol);
    copyConstructorSymbol->SetConstructorOrDestructorSymbol();
    copyConstructorSymbol->SetMemberFunctionSymbol();
    if (!unique)
    {
        copyConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
        copyConstructorSymbol->SetReplicated();
    }
    copyConstructorSymbol->AddSymbol(thisParam);
    copyConstructorSymbol->AddSymbol(thatParam);
    copyConstructorSymbol->ComputeName();
    if (!generateImplementation) return copyConstructorSymbol;
    std::unique_ptr<Cm::BoundTree::BoundFunction> copyConstructor(new Cm::BoundTree::BoundFunction(nullptr, copyConstructorSymbol));
    copyConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), compileUnit.SynthesizedClassFunRepository(),
        copyConstructor.get());
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassConstRefType = compileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(baseClassType, span);
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassConstRefType, thatParam->GetType(), 1, span);
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        Cm::BoundTree::BoundConversion* thatAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThatParam, conversionFun);
        thatAsBase->SetType(baseClassConstRefType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thatAsBase);
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = GenerateBaseConstructorCall(span, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate copy constructor", exception);
        if (initBaseClasObjectStatement)
        {
            copyConstructor->Body()->AddStatement(initBaseClasObjectStatement);
        }
        else
        {
            return nullptr;
        }
    }
    if (classTypeSymbol->IsVirtual())
    {
        copyConstructor->Body()->AddStatement(new Cm::BoundTree::BoundInitVPtrStatement(classTypeSymbol));
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thatMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thatMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        thatMemberVarArg->SetClassObject(boundThatParam);
        arguments.Add(thatMemberVarArg);
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = GenerateInitMemberVariableStatement(span, compileUnit, classTypeSymbol, thisParam, memberVariableSymbol, arguments,
            "cannot generate copy constructor", exception);
        if (initMemberVariableStatement)
        {
            copyConstructor->Body()->AddStatement(initMemberVariableStatement);
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(copyConstructor.release());
    return copyConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateMoveConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    return nullptr;
}

Cm::Sym::FunctionSymbol* GenerateCopyAssignment(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::TypeSymbol* constRefType = compileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thatParam = new Cm::Sym::ParameterSymbol(span, "that");
    thatParam->SetType(constRefType);
    Cm::Sym::FunctionSymbol* copyAssignmentSymbol = new Cm::Sym::FunctionSymbol(span, "@copy_assignment");
    copyAssignmentSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    copyAssignmentSymbol->SetGroupName("operator=");
    copyAssignmentSymbol->SetParent(classTypeSymbol);
    copyAssignmentSymbol->SetMemberFunctionSymbol();
    if (!unique)
    {
        copyAssignmentSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
        copyAssignmentSymbol->SetReplicated();
    }
    copyAssignmentSymbol->AddSymbol(thisParam);
    copyAssignmentSymbol->AddSymbol(thatParam);
    Cm::Sym::TypeSymbol* voidType = compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    copyAssignmentSymbol->SetReturnType(voidType);
    copyAssignmentSymbol->ComputeName();
    if (!generateImplementation) return copyAssignmentSymbol;
    std::unique_ptr<Cm::BoundTree::BoundFunction> copyAssignment(new Cm::BoundTree::BoundFunction(nullptr, copyAssignmentSymbol));
    copyAssignment->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), compileUnit.SynthesizedClassFunRepository(),
        copyAssignment.get());
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassConstRefType = compileUnit.SymbolTable().GetTypeRepository().MakeConstReferenceType(baseClassType, span);
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassConstRefType, thatParam->GetType(), 1, span);
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        Cm::BoundTree::BoundConversion* thatAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThatParam, conversionFun);
        thatAsBase->SetType(baseClassConstRefType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thatAsBase);
        Cm::BoundTree::BoundFunctionCallStatement* assignBaseClasObjectStatement = GenerateBaseAssignmentCall(span, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate copy assignment", exception);
        if (assignBaseClasObjectStatement)
        {
            copyAssignment->Body()->AddStatement(assignBaseClasObjectStatement);
        }
        else
        {
            return nullptr;
        }
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thatMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thatMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        thatMemberVarArg->SetClassObject(boundThatParam);
        arguments.Add(thatMemberVarArg);
        Cm::BoundTree::BoundFunctionCallStatement* assignMemberVariableStatement = GenerateAssignMemberVariableStatement(span, compileUnit, classTypeSymbol, thisParam, memberVariableSymbol, arguments,
            "cannot generate copy assignment", exception);
        if (assignMemberVariableStatement)
        {
            copyAssignment->Body()->AddStatement(assignMemberVariableStatement);
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(copyAssignment.release());
    return copyAssignmentSymbol;
}

Cm::Sym::FunctionSymbol* GenerateMoveAssignment(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    return nullptr;
}

Cm::Sym::FunctionSymbol* GenerateDestructorSymbol(Cm::Sym::SymbolTable& symbolTable, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Ast::CompileUnitNode* compileUnit)
{
    Cm::Sym::TypeSymbol* classTypePointer = symbolTable.GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::FunctionSymbol* destructorSymbol = new Cm::Sym::FunctionSymbol(span, "@dtor");
    if (classTypeSymbol->IsVirtual())
    {
        if (classTypeSymbol->BaseClass() && classTypeSymbol->BaseClass()->IsVirtual())
        {
            destructorSymbol->SetOverride();
        }
        else
        {
            destructorSymbol->SetVirtual();
        }
    }
    destructorSymbol->SetCompileUnit(compileUnit);
    destructorSymbol->SetGroupName("@destructor");
    destructorSymbol->SetParent(classTypeSymbol);
    destructorSymbol->SetConstructorOrDestructorSymbol();
    destructorSymbol->SetMemberFunctionSymbol();
    destructorSymbol->AddSymbol(thisParam);
    destructorSymbol->ComputeName();
    return destructorSymbol; 
}

void GenerateDestructorImplementation(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Cm::Sym::FunctionSymbol* destructorSymbol = classTypeSymbol->Destructor();
    Cm::Sym::ParameterSymbol* thisParam = destructorSymbol->Parameters()[0];
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    std::unique_ptr<Cm::BoundTree::BoundFunction> destructor(new Cm::BoundTree::BoundFunction(nullptr, destructorSymbol));
    destructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(compileUnit.SymbolTable(), compileUnit.ConversionTable(), compileUnit.ClassConversionTable(), compileUnit.DerivedTypeOpRepository(), compileUnit.SynthesizedClassFunRepository(),
        destructor.get());
    if (classTypeSymbol->IsVirtual())
    {
        destructor->Body()->AddStatement(new Cm::BoundTree::BoundInitVPtrStatement(classTypeSymbol));
    }
    int n = int(classTypeSymbol->MemberVariables().size());
    for (int i = n - 1; i >= 0; --i)
    {
        Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classTypeSymbol->MemberVariables()[i];
        Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
        if (!memberVariableType->IsClassTypeSymbol()) continue;
        Cm::Sym::ClassTypeSymbol* memberVariableClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
        if (!memberVariableClassType->Destructor()) continue;
        Cm::Sym::FunctionSymbol* memberDtor = memberVariableClassType->Destructor();
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thisMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thisMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        thisMemberVarArg->SetClassObject(boundThisParam);
        arguments.Add(thisMemberVarArg);
        PrepareFunctionArguments(memberDtor, arguments, true, compileUnit.IrClassTypeRepository());
        Cm::BoundTree::BoundFunctionCallStatement* destroyMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberDtor, std::move(arguments));
        destructor->Body()->AddStatement(destroyMemberVariableStatement);
    }
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        Cm::Sym::FunctionSymbol* baseClassDtor = baseClass->Destructor();
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassPtrType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClassType, span);
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
        Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
        thisAsBase->SetType(baseClassPtrType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thisAsBase);
        PrepareFunctionArguments(baseClassDtor, arguments, true, compileUnit.IrClassTypeRepository());
        Cm::BoundTree::BoundFunctionCallStatement* destroyBaseClassObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassDtor, std::move(arguments));
        destructor->Body()->AddStatement(destroyBaseClassObjectStatement);
    }
    compileUnit.AddBoundNode(destructor.release());
}

void GenerateSynthesizedFunctionImplementation(Cm::Sym::FunctionSymbol* function, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Cm::Core::Exception* exception = nullptr;
    if (classTypeSymbol->GenerateDefaultConstructor() && function->IsDefaultConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateDefaultConstructor(true, true, span, classTypeSymbol, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateCopyConstructor() && function->IsCopyConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateCopyConstructor(true, true, span, classTypeSymbol, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateMoveConstructor() && function->IsMoveConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateMoveConstructor(true, true, span, classTypeSymbol, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateCopyAssignment() && function->IsCopyAssignment())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateCopyAssignment(true, true, span, classTypeSymbol, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateMoveAssignment() && function->IsMoveAssignment())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateMoveAssignment(true, true, span, classTypeSymbol, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    if (exception)
    {
        Cm::Core::Exception copyOfEx = *exception;
        delete exception;
        throw copyOfEx;
    }
}

SynthesizedClassFunCache::SynthesizedClassFunCache()
{
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetDefaultConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    if (!defaultConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateDefaultConstructor(); // default implementation is generated from elsewhere
        defaultConstructor.reset(GenerateDefaultConstructor(generateImplementation, classTypeSymbol->GenerateDefaultConstructor(), span, classTypeSymbol, compileUnit, exception));
    }
    return defaultConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetCopyConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    if (!copyConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateCopyConstructor(); // default implementation is generated from elsewhere
        copyConstructor.reset(GenerateCopyConstructor(generateImplementation, classTypeSymbol->GenerateCopyConstructor(), span, classTypeSymbol, compileUnit, exception));
    }
    return copyConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetMoveConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    if (!moveConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateMoveConstructor(); // default implementation is generated from elsewhere
        moveConstructor.reset(GenerateMoveConstructor(generateImplementation, classTypeSymbol->GenerateMoveConstructor(), span, classTypeSymbol, compileUnit, exception));
    }
    return moveConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetCopyAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    if (!copyAssignment)
    {
        bool generateImplementation = !classTypeSymbol->GenerateCopyAssignment(); // default implementation is generated from elsewhere
        copyAssignment.reset(GenerateCopyAssignment(generateImplementation, classTypeSymbol->GenerateCopyAssignment(), span, classTypeSymbol, compileUnit, exception));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetMoveAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Core::Exception*& exception)
{
    if (!moveAssignment)
    {
        bool generateImplementation = !classTypeSymbol->GenerateMoveAssignment(); // default implementation is generated from elsewhere
        moveAssignment.reset(GenerateMoveAssignment(generateImplementation, classTypeSymbol->GenerateMoveAssignment(), span, classTypeSymbol, compileUnit, exception));
    }
    return moveAssignment.get();
}

SynthesizedClassFunGroup::SynthesizedClassFunGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_) : compileUnit(compileUnit_)
{
}

SynthesizedClassFunGroup::~SynthesizedClassFunGroup()
{
}

SynthesizedConstructorGroup::SynthesizedConstructorGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_) : SynthesizedClassFunGroup(compileUnit_)
{
}

void SynthesizedConstructorGroup::CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, Cm::Core::Exception*& exception)
{
    if (arity == 1)
    {
        if (classType->IsStatic())
        {
            exception = new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan());
        }
        else if (classType->HasSuppressedDefaultConstructor())
        {
            exception = new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because default constructor is suppressed", span, classType->GetSpan());
        }
        else if (classType->HasUserDefinedConstructor())
        {
            exception = new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because class has user defined constructor", span, classType->GetSpan());
        }
        else
        {
            SynthesizedClassFunCache& cache = cacheMap[classType];
            Cm::Sym::FunctionSymbol* defaultConstructor = cache.GetDefaultConstructor(span, classType, CompileUnit(), exception);
            if (defaultConstructor)
            {
                viableFunctions.insert(defaultConstructor);
            }
        }
    }
    else if (arity == 2)
    {
        Cm::Sym::TypeSymbol* secondArgumentType = arguments[1].Type();
        if (secondArgumentType->IsPointerType()) return;
        Cm::Sym::TypeSymbol* secondArgumentBaseType = secondArgumentType->GetBaseType();
        if (!secondArgumentType->IsRvalueRefType() && !arguments[1].BindToRvalueRef() && TypesEqual(classType, secondArgumentBaseType))
        {
            if (classType->IsStatic())
            {
                exception = new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan());
            }
            else if (classType->HasSuppressedCopyConstructor())
            {
                exception = new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because copy constructor is suppressed", span, classType->GetSpan());
            }
            else
            {
                bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                    classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
                if (hasUserDefinedCopyOrMoveOperOrDestructor)
                {
                    exception = new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                        span, classType->GetSpan());
                }
                else
                {
                    SynthesizedClassFunCache& cache = cacheMap[classType];
                    Cm::Sym::FunctionSymbol* copyConstructor = cache.GetCopyConstructor(span, classType, CompileUnit(), exception);
                    if (copyConstructor)
                    {
                        viableFunctions.insert(copyConstructor);
                    }
                }
            }
        }
    }
}

SynthesizedAssignmentGroup::SynthesizedAssignmentGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_) : SynthesizedClassFunGroup(compileUnit_)
{
}

void SynthesizedAssignmentGroup::CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments,
    const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, Cm::Core::Exception*& exception)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* secondArgumentType = arguments[1].Type();
    if (secondArgumentType->IsPointerType()) return;
    Cm::Sym::TypeSymbol* secondArgumentBaseType = secondArgumentType->GetBaseType();
    if (!secondArgumentType->IsRvalueRefType() && !arguments[1].BindToRvalueRef() && TypesEqual(classType, secondArgumentBaseType))
    {
        if (classType->IsStatic())
        {
            exception = new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan());
        }
        else if (classType->HasSuppressedCopyConstructor())
        {
            exception = new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because copy assignment is suppressed", span, classType->GetSpan());
        }
        else
        {
            bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
            if (hasUserDefinedCopyOrMoveOperOrDestructor)
            {
                exception = new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                    span, classType->GetSpan());
            }
            else
            {
                SynthesizedClassFunCache& cache = cacheMap[classType];
                Cm::Sym::FunctionSymbol* copyAssignment = cache.GetCopyAssignment(span, classType, CompileUnit(), exception);
                if (copyAssignment)
                {
                    viableFunctions.insert(copyAssignment);
                }
            }
        }
    }
}

SynthesizedClassFunRepository::SynthesizedClassFunRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_) : 
    compileUnit(compileUnit_), synthesizedConstructorGroup(compileUnit_), synthesizedAssignmentGroup(compileUnit_)
{
    synthesizedClassFunGroupMap["@constructor"] = &synthesizedConstructorGroup;
    synthesizedClassFunGroupMap["operator="] = &synthesizedAssignmentGroup;
}

void SynthesizedClassFunRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span,
    std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, Cm::Core::Exception*& exception)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1 || arity > 2) return;
    Cm::Sym::TypeSymbol* leftArgType = arguments[0].Type();
    if (leftArgType->IsReferenceType() || !leftArgType->IsPointerToClassType()) return;
    Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(leftArgType->GetBaseType());
    SynthesizedClassFunGroupMapIt i = synthesizedClassFunGroupMap.find(groupName);
    if (i != synthesizedClassFunGroupMap.end())
    {
        SynthesizedClassFunGroup* group = i->second;
        group->CollectViableFunctions(cacheMap, classType, arity, arguments, span, viableFunctions, exception);
    }
}

} } // namespace Cm::Bind
