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
#include <Cm.Bind/Class.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

Cm::BoundTree::BoundInitClassObjectStatement* GenerateBaseConstructorCall(const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, 
    Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ClassTypeSymbol* baseClassType, Cm::Sym::ParameterSymbol* thisParam, Cm::BoundTree::BoundExpressionList& arguments, 
    const std::string& errorMessageHeader, std::unique_ptr<Cm::Core::Exception>& exception)
{
    AddClassTypeToIrClassTypeRepository(baseClassType, compileUnit, containerScope);
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
        baseClassCtor = ResolveOverload(containerScope, compileUnit, "@constructor", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception.reset(new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because base class constructor not found: " + ex.Message(), ex.Defined(), ex.Referenced()));
        return nullptr;
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    arguments.InsertFront(thisAsBase); // insert 'this' to front
    PrepareArguments(containerScope, compileUnit, nullptr, nullptr, baseClassCtor->Parameters(), arguments, false, compileUnit.IrClassTypeRepository(), baseClassCtor->IsBasicTypeOp());
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
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            arguments[i].reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            arguments[i]->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
    functionCall->SetFunction(baseClassCtor);
    Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    return initBaseClasObjectStatement;
}

Cm::BoundTree::BoundInitMemberVariableStatement* GenerateInitMemberVariableStatement(const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, 
    Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ParameterSymbol* thisParam, Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments, 
    const std::string& errorMessageHeader, std::unique_ptr<Cm::Core::Exception>& exception)
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
        AddClassTypeToIrClassTypeRepository(memberVarClassType, compileUnit, containerScope);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberCtor = nullptr;
    try
    {
        memberCtor = ResolveOverload(containerScope, compileUnit, "@constructor", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception.reset(new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because member variable constructor for member variable '" + memberVariableSymbol->Name() + 
            "' not found: " + ex.Message(), ex.Defined(), ex.Referenced()));
        return nullptr;
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    boundMemberVariable->SetClassObject(boundThisParam);
    arguments.InsertFront(boundMemberVariable);
    PrepareArguments(containerScope, compileUnit, nullptr, nullptr, memberCtor->Parameters(), arguments, true, compileUnit.IrClassTypeRepository(), memberCtor->IsBasicTypeOp());
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
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            arguments[i].reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            arguments[i]->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = new Cm::BoundTree::BoundInitMemberVariableStatement(memberCtor, std::move(arguments));
    initMemberVariableStatement->SetMemberVariableSymbol(memberVariableSymbol);
    return initMemberVariableStatement;
}

Cm::BoundTree::BoundFunctionCallStatement* GenerateBaseAssignmentCall(const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit,
    Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ClassTypeSymbol* baseClassType, Cm::Sym::ParameterSymbol* thisParam, Cm::BoundTree::BoundExpressionList& arguments, 
    const std::string& errorMessageHeader, std::unique_ptr<Cm::Core::Exception>& exception)
{
    AddClassTypeToIrClassTypeRepository(baseClassType, compileUnit, containerScope);
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
        baseClassAssignment = ResolveOverload(containerScope, compileUnit, "operator=", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception.reset(new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because base class copy assignment not found: " + ex.Message(), ex.Defined(), ex.Referenced()));
        return nullptr;
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    arguments.InsertFront(thisAsBase); // insert 'this' to front
    PrepareArguments(containerScope, compileUnit, nullptr, baseClassAssignment->GetReturnType(), baseClassAssignment->Parameters(), arguments, false, compileUnit.IrClassTypeRepository(), baseClassAssignment->IsBasicTypeOp());
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
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            arguments[i].reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            arguments[i]->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCallStatement* assignBaseClasObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassAssignment, std::move(arguments));
    return assignBaseClasObjectStatement;
}

Cm::BoundTree::BoundFunctionCallStatement* GenerateAssignMemberVariableStatement(const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, 
    Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ParameterSymbol* thisParam, Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments, 
    const std::string& errorMessageHeader, std::unique_ptr<Cm::Core::Exception>& exception)
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
        AddClassTypeToIrClassTypeRepository(memberVarClassType, compileUnit, containerScope);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberAssignment = nullptr;
    try
    {
        memberAssignment = ResolveOverload(containerScope, compileUnit, "operator=", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        exception.reset(new Cm::Core::Exception(errorMessageHeader + " for class '" + classTypeSymbol->FullName() + "' because member variable copy assignment for member variable '" + memberVariableSymbol->Name() +
            "' not found: " + ex.Message(), ex.Defined(), ex.Referenced()));
        return nullptr;
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    boundMemberVariable->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    boundMemberVariable->SetClassObject(boundThisParam);
    arguments.InsertFront(boundMemberVariable);
    PrepareArguments(containerScope, compileUnit, nullptr, memberAssignment->GetReturnType(), memberAssignment->Parameters(), arguments, true, compileUnit.IrClassTypeRepository(), memberAssignment->IsBasicTypeOp());
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
            Cm::BoundTree::BoundExpression* arg = arguments[i].release();
            arguments[i].reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
            arguments[i]->SetType(conversionFun->GetTargetType());
        }
    }
    Cm::BoundTree::BoundFunctionCallStatement* assignMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberAssignment, std::move(arguments));
    return assignMemberVariableStatement;
}

Cm::Sym::FunctionSymbol* GenerateDefaultConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
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
    defaultConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    if (!unique)
    {
        defaultConstructorSymbol->SetReplicated();
    }
    defaultConstructorSymbol->AddSymbol(thisParam);
    defaultConstructorSymbol->ComputeName();
    defaultConstructorSymbol->SetNothrow();
    if (!generateImplementation) return defaultConstructorSymbol;
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    std::unique_ptr<Cm::BoundTree::BoundFunction> defaultConstructor(new Cm::BoundTree::BoundFunction(nullptr, defaultConstructorSymbol));
    defaultConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, defaultConstructor.get());
    if (classTypeSymbol->StaticConstructor())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundFunctionCallStatement* staticConstructorCallStatement = new Cm::BoundTree::BoundFunctionCallStatement(classTypeSymbol->StaticConstructor(), std::move(arguments));
        if (classTypeSymbol->StaticConstructor()->CanThrow())
        {
            defaultConstructorSymbol->ResetNothrow();
        }
        defaultConstructor->Body()->AddStatement(staticConstructorCallStatement);
    }
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = GenerateBaseConstructorCall(span, containerScope, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments, 
            "cannot generate default constructor", exception);
        if (initBaseClasObjectStatement)
        {
            defaultConstructor->Body()->AddStatement(initBaseClasObjectStatement);
            if (initBaseClasObjectStatement->FunctionCall()->GetFunction()->CanThrow())
            {
                defaultConstructorSymbol->ResetNothrow();
            }
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
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = GenerateInitMemberVariableStatement(span, containerScope, compileUnit, classTypeSymbol, thisParam, memberVariableSymbol, arguments,
            "cannot generate default constructor", exception);
        if (initMemberVariableStatement)
        {
            defaultConstructor->Body()->AddStatement(initMemberVariableStatement);
            if (initMemberVariableStatement->Constructor()->CanThrow())
            {
                defaultConstructorSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(defaultConstructor.release());
    return defaultConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateCopyConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
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
    copyConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    copyConstructorSymbol->SetNothrow();
    if (!unique)
    {
        copyConstructorSymbol->SetReplicated();
    }
    copyConstructorSymbol->AddSymbol(thisParam);
    copyConstructorSymbol->AddSymbol(thatParam);
    copyConstructorSymbol->ComputeName();
    if (!generateImplementation) return copyConstructorSymbol;
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    std::unique_ptr<Cm::BoundTree::BoundFunction> copyConstructor(new Cm::BoundTree::BoundFunction(nullptr, copyConstructorSymbol));
    copyConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, copyConstructor.get());
    if (classTypeSymbol->StaticConstructor())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundFunctionCallStatement* staticConstructorCallStatement = new Cm::BoundTree::BoundFunctionCallStatement(classTypeSymbol->StaticConstructor(), std::move(arguments));
        copyConstructor->Body()->AddStatement(staticConstructorCallStatement);
        if (classTypeSymbol->StaticConstructor()->CanThrow())
        {
            copyConstructorSymbol->ResetNothrow();
        }
    }
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
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = GenerateBaseConstructorCall(span, containerScope, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate copy constructor", exception);
        if (initBaseClasObjectStatement)
        {
            copyConstructor->Body()->AddStatement(initBaseClasObjectStatement);
            if (initBaseClasObjectStatement->FunctionCall()->GetFunction()->CanThrow())
            {
                copyConstructorSymbol->ResetNothrow();
            }
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
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = GenerateInitMemberVariableStatement(span, containerScope, compileUnit, classTypeSymbol, thisParam, 
            memberVariableSymbol, arguments, "cannot generate copy constructor", exception);
        if (initMemberVariableStatement)
        {
            copyConstructor->Body()->AddStatement(initMemberVariableStatement);
            if (initMemberVariableStatement->Constructor()->CanThrow())
            {
                copyConstructorSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(copyConstructor.release());
    return copyConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateMoveConstructor(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::TypeSymbol* rvalueRefType = compileUnit.SymbolTable().GetTypeRepository().MakeRvalueRefType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thatParam = new Cm::Sym::ParameterSymbol(span, "that");
    thatParam->SetType(rvalueRefType);
    Cm::Sym::FunctionSymbol* moveConstructorSymbol = new Cm::Sym::FunctionSymbol(span, "@move_ctor");
    moveConstructorSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    moveConstructorSymbol->SetGroupName("@constructor");
    moveConstructorSymbol->SetParent(classTypeSymbol);
    moveConstructorSymbol->SetConstructorOrDestructorSymbol();
    moveConstructorSymbol->SetMemberFunctionSymbol();
    moveConstructorSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    moveConstructorSymbol->SetNothrow();
    if (!unique)
    {
        moveConstructorSymbol->SetReplicated();
    }
    moveConstructorSymbol->AddSymbol(thisParam);
    moveConstructorSymbol->AddSymbol(thatParam);
    moveConstructorSymbol->ComputeName();
    if (!generateImplementation) return moveConstructorSymbol;
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    std::unique_ptr<Cm::BoundTree::BoundFunction> moveConstructor(new Cm::BoundTree::BoundFunction(nullptr, moveConstructorSymbol));
    moveConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, moveConstructor.get());
    if (classTypeSymbol->StaticConstructor())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundFunctionCallStatement* staticConstructorCallStatement = new Cm::BoundTree::BoundFunctionCallStatement(classTypeSymbol->StaticConstructor(), std::move(arguments));
        moveConstructor->Body()->AddStatement(staticConstructorCallStatement);
        if (classTypeSymbol->StaticConstructor()->CanThrow())
        {
            moveConstructorSymbol->ResetNothrow();
        }
    }
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassRvalueRefType = compileUnit.SymbolTable().GetTypeRepository().MakeRvalueRefType(baseClassType, span);
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassRvalueRefType, thatParam->GetType(), 1, span);
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        Cm::BoundTree::BoundConversion* thatAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThatParam, conversionFun);
        thatAsBase->SetType(baseClassRvalueRefType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thatAsBase);
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = GenerateBaseConstructorCall(span, containerScope, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate move constructor", exception);
        if (initBaseClasObjectStatement)
        {
            moveConstructor->Body()->AddStatement(initBaseClasObjectStatement);
            if (initBaseClasObjectStatement->FunctionCall()->GetFunction()->CanThrow())
            {
                moveConstructorSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    if (classTypeSymbol->IsVirtual())
    {
        moveConstructor->Body()->AddStatement(new Cm::BoundTree::BoundInitVPtrStatement(classTypeSymbol));
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thatMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thatMemberVarArg->SetType(memberVariableSymbol->GetType());
        thatMemberVarArg->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        thatMemberVarArg->SetClassObject(boundThatParam);
        std::vector<Cm::Core::Argument> rvalueThatResolutionArguments;
        rvalueThatResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, memberVariableSymbol->GetType()));
        Cm::Sym::FunctionLookupSet rvalueThatFunctionLookups;
        rvalueThatFunctionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, containerScope));
        std::vector<Cm::Sym::FunctionSymbol*> rvalueThatConversions;
        Cm::Sym::FunctionSymbol* rvalueFun = ResolveOverload(containerScope, compileUnit, "System.Rvalue", rvalueThatResolutionArguments, rvalueThatFunctionLookups, span, rvalueThatConversions);
        Cm::BoundTree::BoundExpressionList rvalueThatArguments;
        rvalueThatArguments.Add(thatMemberVarArg);
        Cm::BoundTree::BoundFunctionCall* rvalueThat = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(rvalueThatArguments));
        rvalueThat->SetFunction(rvalueFun);
        rvalueThat->SetType(rvalueFun->GetReturnType());
        arguments.Add(rvalueThat);
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = GenerateInitMemberVariableStatement(span, containerScope, compileUnit, classTypeSymbol, thisParam,
            memberVariableSymbol, arguments, "cannot generate move constructor", exception);
        if (initMemberVariableStatement)
        {
            moveConstructor->Body()->AddStatement(initMemberVariableStatement);
            if (initMemberVariableStatement->Constructor()->CanThrow())
            {
                moveConstructorSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(moveConstructor.release());
    return moveConstructorSymbol;
}

Cm::Sym::FunctionSymbol* GenerateCopyAssignment(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
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
    copyAssignmentSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    copyAssignmentSymbol->SetNothrow();
    if (!unique)
    {
        copyAssignmentSymbol->SetReplicated();
    }
    copyAssignmentSymbol->AddSymbol(thisParam);
    copyAssignmentSymbol->AddSymbol(thatParam);
    Cm::Sym::TypeSymbol* voidType = compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    copyAssignmentSymbol->SetReturnType(voidType);
    copyAssignmentSymbol->ComputeName();
    if (!generateImplementation) return copyAssignmentSymbol;
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    std::unique_ptr<Cm::BoundTree::BoundFunction> copyAssignment(new Cm::BoundTree::BoundFunction(nullptr, copyAssignmentSymbol));
    copyAssignment->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, copyAssignment.get());
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
        Cm::BoundTree::BoundFunctionCallStatement* assignBaseClasObjectStatement = GenerateBaseAssignmentCall(span, containerScope, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate copy assignment", exception);
        if (assignBaseClasObjectStatement)
        {
            copyAssignment->Body()->AddStatement(assignBaseClasObjectStatement);
            if (assignBaseClasObjectStatement->Function()->CanThrow())
            {
                copyAssignmentSymbol->ResetNothrow();
            }
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
        Cm::BoundTree::BoundFunctionCallStatement* assignMemberVariableStatement = GenerateAssignMemberVariableStatement(span, containerScope, compileUnit, classTypeSymbol, thisParam, 
            memberVariableSymbol, arguments, "cannot generate copy assignment", exception);
        if (assignMemberVariableStatement)
        {
            copyAssignment->Body()->AddStatement(assignMemberVariableStatement);
            if (assignMemberVariableStatement->Function()->CanThrow())
            {
                copyAssignmentSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    compileUnit.AddBoundNode(copyAssignment.release());
    return copyAssignmentSymbol;
}

Cm::Sym::FunctionSymbol* GenerateMoveAssignment(bool generateImplementation, bool unique, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thisParam = new Cm::Sym::ParameterSymbol(span, "this");
    thisParam->SetType(classTypePointer);
    Cm::Sym::TypeSymbol* rvalueRefType = compileUnit.SymbolTable().GetTypeRepository().MakeRvalueRefType(classTypeSymbol, span);
    Cm::Sym::ParameterSymbol* thatParam = new Cm::Sym::ParameterSymbol(span, "that");
    thatParam->SetType(rvalueRefType);
    Cm::Sym::FunctionSymbol* moveAssignmentSymbol = new Cm::Sym::FunctionSymbol(span, "@move_assignment");
    moveAssignmentSymbol->SetCompileUnit(compileUnit.SyntaxUnit());
    moveAssignmentSymbol->SetGroupName("operator=");
    Cm::Sym::TypeSymbol* voidType = compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
    moveAssignmentSymbol->SetReturnType(voidType);
    moveAssignmentSymbol->SetParent(classTypeSymbol);
    moveAssignmentSymbol->SetMemberFunctionSymbol();
    moveAssignmentSymbol->SetAccess(Cm::Sym::SymbolAccess::public_);
    moveAssignmentSymbol->SetNothrow();
    if (!unique)
    {
        moveAssignmentSymbol->SetReplicated();
    }
    moveAssignmentSymbol->AddSymbol(thisParam);
    moveAssignmentSymbol->AddSymbol(thatParam);
    moveAssignmentSymbol->ComputeName();
    if (!generateImplementation) return moveAssignmentSymbol;
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    std::unique_ptr<Cm::BoundTree::BoundFunction> moveAssignment(new Cm::BoundTree::BoundFunction(nullptr, moveAssignmentSymbol));
    moveAssignment->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, moveAssignment.get());
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassRvalueRefType = compileUnit.SymbolTable().GetTypeRepository().MakeRvalueRefType(baseClassType, span);
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassRvalueRefType, thatParam->GetType(), 1, span);
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        Cm::BoundTree::BoundConversion* thatAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThatParam, conversionFun);
        thatAsBase->SetType(baseClassRvalueRefType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thatAsBase);
        Cm::BoundTree::BoundFunctionCallStatement* assignBaseClasObjectStatement = GenerateBaseAssignmentCall(span, containerScope, compileUnit, classTypeSymbol, baseClassType, thisParam, arguments,
            "cannot generate move assignment", exception);
        if (assignBaseClasObjectStatement)
        {
            moveAssignment->Body()->AddStatement(assignBaseClasObjectStatement);
            if (assignBaseClasObjectStatement->Function()->CanThrow())
            {
                moveAssignmentSymbol->ResetNothrow();
            }
        }
        else
        {
            return nullptr;
        }
    }
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thisMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thisMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        thisMemberVarArg->SetClassObject(boundThisParam);
        thisMemberVarArg->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        arguments.Add(thisMemberVarArg);
        Cm::BoundTree::BoundMemberVariable* thatMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thatMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThatParam = new Cm::BoundTree::BoundParameter(nullptr, thatParam);
        boundThatParam->SetType(thatParam->GetType());
        thatMemberVarArg->SetClassObject(boundThatParam);
        thatMemberVarArg->SetFlag(Cm::BoundTree::BoundNodeFlags::argByRef);
        arguments.Add(thatMemberVarArg);
        std::vector<Cm::Core::Argument> resolutionArguments;
        for (std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
        {
            resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
        }
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, containerScope));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* swapFun = ResolveOverload(containerScope, compileUnit, "System.Swap", resolutionArguments, functionLookups, span, conversions);
        Cm::BoundTree::BoundFunctionCallStatement* swapStatement = new Cm::BoundTree::BoundFunctionCallStatement(swapFun, std::move(arguments));
        moveAssignment->Body()->AddStatement(swapStatement);
    }
    compileUnit.AddBoundNode(moveAssignment.release());
    return moveAssignmentSymbol;
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
    destructorSymbol->SetNothrow();
    destructorSymbol->SetPublic();
    return destructorSymbol; 
}

Cm::Sym::FunctionSymbol* GenerateStaticConstructorSymbol(Cm::Sym::SymbolTable& symbolTable, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Ast::CompileUnitNode* compileUnit)
{
    Cm::Sym::FunctionSymbol* staticConstructorSymbol = new Cm::Sym::FunctionSymbol(span, "@static_ctor");
    staticConstructorSymbol->SetStatic();
    staticConstructorSymbol->SetCompileUnit(compileUnit);
    staticConstructorSymbol->SetGroupName("@static_constructor");
    staticConstructorSymbol->SetParent(classTypeSymbol);
    staticConstructorSymbol->SetConstructorOrDestructorSymbol();
    staticConstructorSymbol->SetMemberFunctionSymbol();
    staticConstructorSymbol->ComputeName();
    staticConstructorSymbol->SetNothrow();
    staticConstructorSymbol->SetPublic();
    staticConstructorSymbol->SetMutexId(Cm::Sym::GetMutexTable()->GetNextMutexId());
    return staticConstructorSymbol;
}

void GenerateDestructorImplementation(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Cm::Sym::FunctionSymbol* destructorSymbol = classTypeSymbol->Destructor();
    if (compileUnit.Instantiated(destructorSymbol)) return;
    compileUnit.AddToInstantiated(destructorSymbol);
    AddClassTypeToIrClassTypeRepository(classTypeSymbol, compileUnit, containerScope);
    Cm::Sym::ParameterSymbol* thisParam = destructorSymbol->Parameters()[0];
    Cm::Sym::TypeSymbol* classTypePointer = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(classTypeSymbol, span);
    std::unique_ptr<Cm::BoundTree::BoundFunction> destructor(new Cm::BoundTree::BoundFunction(nullptr, destructorSymbol));
    destructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));
    GenerateReceives(containerScope, compileUnit, destructor.get());
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
        if (memberVariableClassType->IsTemplateTypeSymbol() && !memberVariableClassType->Bound())
        {
            compileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(static_cast<Cm::Sym::TemplateTypeSymbol*>(memberVariableClassType), containerScope, compileUnit.GetFileScopes());
        }
        if (!memberVariableClassType->Destructor()) continue;
        Cm::Sym::FunctionSymbol* memberDtor = memberVariableClassType->Destructor();
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundMemberVariable* thisMemberVarArg = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        thisMemberVarArg->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        thisMemberVarArg->SetClassObject(boundThisParam);
        arguments.Add(thisMemberVarArg);
        PrepareArguments(containerScope, compileUnit, nullptr, nullptr, memberDtor->Parameters(), arguments, true, compileUnit.IrClassTypeRepository(), memberDtor->IsBasicTypeOp());
        Cm::BoundTree::BoundFunctionCallStatement* destroyMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberDtor, std::move(arguments));
        destructor->Body()->AddStatement(destroyMemberVariableStatement);
    }
    if (classTypeSymbol->BaseClass())
    {
        Cm::Sym::ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        if (baseClass->IsTemplateTypeSymbol() && !baseClass->Bound())
        {
            compileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(static_cast<Cm::Sym::TemplateTypeSymbol*>(baseClass), containerScope, compileUnit.GetFileScopes());
        }
        Cm::Sym::FunctionSymbol* baseClassDtor = baseClass->Destructor();
        if (!baseClassDtor)
        {
            throw std::runtime_error("could not generate destructor because base class has no destructor");
        }
        Cm::Sym::ClassTypeSymbol* baseClassType = classTypeSymbol->BaseClass();
        Cm::Sym::TypeSymbol* baseClassPtrType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClassType, span);
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        Cm::Sym::FunctionSymbol* conversionFun = compileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
        Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
        thisAsBase->SetType(baseClassPtrType);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(thisAsBase);
        PrepareArguments(containerScope, compileUnit, nullptr, nullptr, baseClassDtor->Parameters(), arguments, true, compileUnit.IrClassTypeRepository(), baseClassDtor->IsBasicTypeOp());
        Cm::BoundTree::BoundFunctionCallStatement* destroyBaseClassObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassDtor, std::move(arguments));
        destructor->Body()->AddStatement(destroyBaseClassObjectStatement);
    }
    compileUnit.AddBoundNode(destructor.release());
}

void GenerateStaticConstructorImplementation(Cm::BoundTree::BoundClass* boundClass, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span, 
	Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Cm::Sym::FunctionSymbol* staticConstructorSymbol = classTypeSymbol->StaticConstructor();
    if (compileUnit.Instantiated(staticConstructorSymbol)) return;
    compileUnit.AddToInstantiated(staticConstructorSymbol);
    compileUnit.IrClassTypeRepository().AddClassType(classTypeSymbol);
    std::unique_ptr<Cm::BoundTree::BoundFunction> staticConstructor(new Cm::BoundTree::BoundFunction(nullptr, staticConstructorSymbol));
    staticConstructor->SetBody(new Cm::BoundTree::BoundCompoundStatement(nullptr));

	Cm::Sym::TypeSymbol* intType = compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
	Cm::Sym::Symbol* mutexGuardSymbol = compileUnit.SymbolTable().GlobalScope()->Lookup("System.Support.MtxGuard");
	if (!mutexGuardSymbol)
	{
		throw std::runtime_error("System.Support.MtxGuard class not found");
	}
	if (!mutexGuardSymbol->IsClassTypeSymbol())
	{
		throw std::runtime_error("System.Support.MtxGuard is not of class type");
	}
	Cm::Sym::ClassTypeSymbol* mutexGuardClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(mutexGuardSymbol);
    AddClassTypeToIrClassTypeRepository(mutexGuardClassType, compileUnit, containerScope);
    std::vector<Cm::Core::Argument> mutexGuardResolutionArguments;
	Cm::Sym::TypeSymbol* mutexGuardPointerType = compileUnit.SymbolTable().GetTypeRepository().MakePointerType(mutexGuardClassType, span);
	mutexGuardResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, mutexGuardPointerType));
	mutexGuardResolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, intType));
	Cm::Sym::FunctionLookupSet mutexGuardFunctionLookups;
	mutexGuardFunctionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, mutexGuardClassType->ClassOrNs()->GetContainerScope()));
	std::vector<Cm::Sym::FunctionSymbol*> mutexGuardConversions;
	Cm::Sym::FunctionSymbol* mutexGuardConstructor = ResolveOverload(containerScope, compileUnit, "@constructor", mutexGuardResolutionArguments, mutexGuardFunctionLookups,
		span, mutexGuardConversions);
	Cm::BoundTree::BoundConstructionStatement* constructMutexGuardStatement = new Cm::BoundTree::BoundConstructionStatement(nullptr);
    int mutexId = staticConstructorSymbol->GetMutexId();
    if (mutexId == -1)
    {
        throw std::runtime_error("invalid mutex id");
    }
	Cm::BoundTree::BoundLiteral* mutexIdLiteral = new Cm::BoundTree::BoundLiteral(nullptr);
	mutexIdLiteral->SetValue(new Cm::Sym::IntValue(mutexId));
	mutexIdLiteral->SetType(intType);
	Cm::BoundTree::BoundExpressionList constructMutexGuardArguments;
	constructMutexGuardArguments.Add(mutexIdLiteral);
	Cm::Sym::LocalVariableSymbol* mutexGuardVar = new Cm::Sym::LocalVariableSymbol(span, "mtxGuard");
    staticConstructor->AddLocalVariable(mutexGuardVar);
    mutexGuardVar->SetType(mutexGuardClassType);
	constructMutexGuardStatement->SetLocalVariable(mutexGuardVar);
	constructMutexGuardStatement->SetArguments(std::move(constructMutexGuardArguments));
	constructMutexGuardStatement->SetConstructor(mutexGuardConstructor);
	constructMutexGuardStatement->InsertLocalVariableToArguments();
	constructMutexGuardStatement->Arguments()[0]->SetFlag(Cm::BoundTree::BoundNodeFlags::constructVariable);
	staticConstructor->Body()->AddStatement(constructMutexGuardStatement);

    Cm::Sym::MemberVariableSymbol* initializedVar = new Cm::Sym::MemberVariableSymbol(span, Cm::IrIntf::GetPrivateSeparator() + "initialized");
    initializedVar->SetParent(classTypeSymbol);
    Cm::Sym::TypeSymbol* boolType = compileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    initializedVar->SetType(boolType);
    initializedVar->SetStatic();
    classTypeSymbol->SetInitializedVar(initializedVar);
    Cm::BoundTree::BoundConditionalStatement* checkInitializedStatement = new Cm::BoundTree::BoundConditionalStatement(nullptr);
    Cm::BoundTree::BoundMemberVariable* boundInitializedVar = new Cm::BoundTree::BoundMemberVariable(nullptr, initializedVar);
    boundInitializedVar->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    checkInitializedStatement->SetCondition(boundInitializedVar);
    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(nullptr);
    checkInitializedStatement->AddStatement(returnStatement);
    staticConstructor->Body()->AddStatement(checkInitializedStatement);
    Cm::BoundTree::BoundMemberVariable* boundInitializedVarLeft = new Cm::BoundTree::BoundMemberVariable(nullptr, initializedVar);
    boundInitializedVarLeft->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    Cm::BoundTree::BoundLiteral* boundTrue = new Cm::BoundTree::BoundLiteral(nullptr);
    boundTrue->SetValue(new Cm::Sym::BoolValue(true));
    boundTrue->SetType(boolType);
    std::vector<Cm::Sym::FunctionSymbol*> boolAssignConversions;
    std::vector<Cm::Core::Argument> boolAssignArgs;
    boolAssignArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, compileUnit.SymbolTable().GetTypeRepository().MakePointerType(boolType, span)));
    boolAssignArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
    Cm::Sym::FunctionLookupSet boolAssignLookups;
    boolAssignLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, containerScope));
    Cm::Sym::FunctionSymbol* boolAssignment = ResolveOverload(containerScope, compileUnit, "operator=", boolAssignArgs, boolAssignLookups, span, boolAssignConversions);
    Cm::BoundTree::BoundAssignmentStatement* setInitializedStatement = new Cm::BoundTree::BoundAssignmentStatement(nullptr, boundInitializedVarLeft, boundTrue, boolAssignment);
    staticConstructor->Body()->AddStatement(setInitializedStatement);

    if (classTypeSymbol->BaseClass() && classTypeSymbol->BaseClass()->StaticConstructor())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(nullptr, std::move(arguments));
        functionCall->SetFunction(classTypeSymbol->BaseClass()->StaticConstructor());
        Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
        staticConstructor->Body()->AddStatement(initBaseClasObjectStatement);
    }

    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->StaticMemberVariables())
    {
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
        Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, compileUnit.SymbolTable().GetTypeRepository().MakePointerType(memberVariableType, span));
        resolutionArguments.push_back(variableArgument);
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, memberVariableType->GetContainerScope()->ClassOrNsScope()));
        if (memberVariableType->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
            AddClassTypeToIrClassTypeRepository(memberVarClassType, compileUnit, containerScope);
        }

        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* memberCtor = nullptr;
        try
        {
            memberCtor = ResolveOverload(containerScope, compileUnit, "@constructor", resolutionArguments, functionLookups, span, conversions);
        }
        catch (const Cm::Core::Exception& ex)
        {
            throw Cm::Core::Exception("constructor for member variable '" + memberVariableSymbol->Name() + "' not found: " + ex.Message(), ex.Defined(), ex.Referenced());
        }
        Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(nullptr, memberVariableSymbol);
        boundMemberVariable->SetType(memberVariableSymbol->GetType());
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(boundMemberVariable);
        PrepareArguments(containerScope, compileUnit, nullptr, nullptr, memberCtor->Parameters(), arguments, true, compileUnit.IrClassTypeRepository(), memberCtor->IsBasicTypeOp());
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
                Cm::BoundTree::BoundExpression* arg = arguments[i].release();
                arguments[i].reset(new Cm::BoundTree::BoundConversion(arg->SyntaxNode(), arg, conversionFun));
                arguments[i]->SetType(conversionFun->GetTargetType());
            }
        }
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = new Cm::BoundTree::BoundInitMemberVariableStatement(memberCtor, std::move(arguments));
        staticConstructor->Body()->AddStatement(initMemberVariableStatement);
    }

    boundClass->AddBoundNode(staticConstructor.release());
}

void GenerateSynthesizedFunctionImplementation(Cm::Sym::FunctionSymbol* function, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, bool unique)
{
    std::unique_ptr<Cm::Core::Exception> exception = nullptr;
    if (classTypeSymbol->GenerateDefaultConstructor() && function->IsDefaultConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateDefaultConstructor(true, unique, span, classTypeSymbol, containerScope, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateCopyConstructor() && function->IsCopyConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateCopyConstructor(true, unique, span, classTypeSymbol, containerScope, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateMoveConstructor() && function->IsMoveConstructor())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateMoveConstructor(true, unique, span, classTypeSymbol, containerScope, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateCopyAssignment() && function->IsCopyAssignment())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateCopyAssignment(true, unique, span, classTypeSymbol, containerScope, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    else if (classTypeSymbol->GenerateMoveAssignment() && function->IsMoveAssignment())
    {
        Cm::Sym::FunctionSymbol* functionSymbol = GenerateMoveAssignment(true, unique, span, classTypeSymbol, containerScope, compileUnit, exception);
        compileUnit.Own(functionSymbol);
    }
    if (exception)
    {
        Cm::Core::Exception copyOfEx = *exception;
        throw copyOfEx;
    }
}

SynthesizedClassFunCache::SynthesizedClassFunCache()
{
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetDefaultConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (!defaultConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateDefaultConstructor(); // default implementation is generated from elsewhere
        if (compileUnit.IsPrebindCompileUnit()) generateImplementation = false;
        defaultConstructor.reset(GenerateDefaultConstructor(generateImplementation, classTypeSymbol->GenerateDefaultConstructor(), span, classTypeSymbol, containerScope, compileUnit, exception));
    }
    return defaultConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetCopyConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (!copyConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateCopyConstructor(); // default implementation is generated from elsewhere
        if (compileUnit.IsPrebindCompileUnit()) generateImplementation = false;
        copyConstructor.reset(GenerateCopyConstructor(generateImplementation, classTypeSymbol->GenerateCopyConstructor(), span, classTypeSymbol, containerScope, compileUnit, exception));
    }
    return copyConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetMoveConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (!moveConstructor)
    {
        bool generateImplementation = !classTypeSymbol->GenerateMoveConstructor(); // default implementation is generated from elsewhere
        if (compileUnit.IsPrebindCompileUnit()) generateImplementation = false;
        moveConstructor.reset(GenerateMoveConstructor(generateImplementation, classTypeSymbol->GenerateMoveConstructor(), span, classTypeSymbol, containerScope, compileUnit, exception));
    }
    return moveConstructor.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetCopyAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (!copyAssignment)
    {
        bool generateImplementation = !classTypeSymbol->GenerateCopyAssignment(); // default implementation is generated from elsewhere
        if (compileUnit.IsPrebindCompileUnit()) generateImplementation = false;
        copyAssignment.reset(GenerateCopyAssignment(generateImplementation, classTypeSymbol->GenerateCopyAssignment(), span, classTypeSymbol, containerScope, compileUnit, exception));
    }
    return copyAssignment.get();
}

Cm::Sym::FunctionSymbol* SynthesizedClassFunCache::GetMoveAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (!moveAssignment)
    {
        bool generateImplementation = !classTypeSymbol->GenerateMoveAssignment(); // default implementation is generated from elsewhere
        if (compileUnit.IsPrebindCompileUnit()) generateImplementation = false;
        moveAssignment.reset(GenerateMoveAssignment(generateImplementation, classTypeSymbol->GenerateMoveAssignment(), span, classTypeSymbol, containerScope, compileUnit, exception));
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
    const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception)
{
    bool generated = false;
    if (arity == 1)
    {
        if (classType->IsStatic())
        {
            exception.reset(new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan()));
        }
        else if (classType->HasSuppressedDefaultConstructor())
        {
            exception.reset(new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because default constructor is suppressed", span, classType->GetSpan()));
        }
        else if (classType->HasUserDefinedConstructor())
        {
            exception.reset(new Cm::Core::Exception("cannot generate default constructor for class '" + classType->FullName() + "' because class has user defined constructor", span, classType->GetSpan()));
        }
        else
        {
            SynthesizedClassFunCache& cache = cacheMap[classType];
            Cm::Sym::FunctionSymbol* defaultConstructor = cache.GetDefaultConstructor(span, classType, containerScope, CompileUnit(), exception);
            if (defaultConstructor)
            {
                viableFunctions.insert(defaultConstructor);
                generated = true;
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
                exception.reset(new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan()));
            }
            else if (classType->HasSuppressedCopyConstructor())
            {
                exception.reset(new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because copy constructor is suppressed", span, classType->GetSpan()));
            }
            else
            {
                bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                    classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
                if (hasUserDefinedCopyOrMoveOperOrDestructor)
                {
                    exception.reset(new Cm::Core::Exception("cannot generate copy constructor for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                        span, classType->GetSpan()));
                }
                else
                {
                    SynthesizedClassFunCache& cache = cacheMap[classType];
                    Cm::Sym::FunctionSymbol* copyConstructor = cache.GetCopyConstructor(span, classType, containerScope, CompileUnit(), exception);
                    if (copyConstructor)
                    {
                        viableFunctions.insert(copyConstructor);
                        generated = true;
                    }
                }
            }
        }
        else if ((secondArgumentType->IsRvalueRefType() || arguments[1].BindToRvalueRef()) && TypesEqual(classType, secondArgumentBaseType))
        {
            if (classType->IsStatic())
            {
                exception.reset(new Cm::Core::Exception("cannot generate move constructor for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan()));
            }
            else if (classType->HasSuppressedMoveConstructor())
            {
                exception.reset(new Cm::Core::Exception("cannot generate move constructor for class '" + classType->FullName() + "' because move constructor is suppressed", span, classType->GetSpan()));
            }
            else
            {
                bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                    classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
                if (hasUserDefinedCopyOrMoveOperOrDestructor)
                {
                    exception.reset(new Cm::Core::Exception("cannot generate move constructor for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                        span, classType->GetSpan()));
                }
                else
                {
                    SynthesizedClassFunCache& cache = cacheMap[classType];
                    Cm::Sym::FunctionSymbol* moveConstructor = cache.GetMoveConstructor(span, classType, containerScope, CompileUnit(), exception);
                    if (moveConstructor)
                    {
                        viableFunctions.insert(moveConstructor);
                        generated = true;
                    }
                }
            }
        }
    }
    if (generated && classType->IsTemplateTypeSymbol())
    {
        CompileUnit().ClassTemplateRepository().InstantiateVirtualFunctionsFor(containerScope, classType);
    }
}

SynthesizedAssignmentGroup::SynthesizedAssignmentGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_) : SynthesizedClassFunGroup(compileUnit_)
{
}

void SynthesizedAssignmentGroup::CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments,
    const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (arity != 2) return;
    Cm::Sym::TypeSymbol* secondArgumentType = arguments[1].Type();
    if (secondArgumentType->IsPointerType()) return;
    Cm::Sym::TypeSymbol* secondArgumentBaseType = secondArgumentType->GetBaseType();
    if (!secondArgumentType->IsRvalueRefType() && !arguments[1].BindToRvalueRef() && TypesEqual(classType, secondArgumentBaseType))
    {
        if (classType->IsStatic())
        {
            exception.reset(new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan()));
        }
        else if (classType->HasSuppressedCopyConstructor())
        {
            exception.reset(new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because copy assignment is suppressed", span, classType->GetSpan()));
        }
        else
        {
            bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
            if (hasUserDefinedCopyOrMoveOperOrDestructor)
            {
                exception.reset(new Cm::Core::Exception("cannot generate copy assignment for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                    span, classType->GetSpan()));
            }
            else
            {
                SynthesizedClassFunCache& cache = cacheMap[classType];
                Cm::Sym::FunctionSymbol* copyAssignment = cache.GetCopyAssignment(span, classType, containerScope, CompileUnit(), exception);
                if (copyAssignment)
                {
                    viableFunctions.insert(copyAssignment);
                }
            }
        }
    }
    else if ((secondArgumentType->IsRvalueRefType() || arguments[1].BindToRvalueRef()) && TypesEqual(classType, secondArgumentBaseType))
    {
        if (classType->IsStatic())
        {
            exception.reset(new Cm::Core::Exception("cannot generate move assignment for class '" + classType->FullName() + "' because class is static", span, classType->GetSpan()));
        }
        else if (classType->HasSuppressedMoveAssignment())
        {
            exception.reset(new Cm::Core::Exception("cannot generate move assignment for class '" + classType->FullName() + "' because move assignment is suppressed", span, classType->GetSpan()));
        }
        else
        {
            bool hasUserDefinedCopyOrMoveOperOrDestructor = classType->HasUserDefinedCopyConstructor() || classType->HasUserDefinedMoveConstructor() || classType->HasUserDefinedCopyAssignment() ||
                classType->HasUserDefinedMoveAssignment() || classType->HasUserDefinedDestructor();
            if (hasUserDefinedCopyOrMoveOperOrDestructor)
            {
                exception.reset(new Cm::Core::Exception("cannot generate move assignment for class '" + classType->FullName() + "' because class has user defined copy or move operation or destructor",
                    span, classType->GetSpan()));
            }
            else
            {
                SynthesizedClassFunCache& cache = cacheMap[classType];
                Cm::Sym::FunctionSymbol* moveAssignment = cache.GetMoveAssignment(span, classType, containerScope, CompileUnit(), exception);
                if (moveAssignment)
                {
                    viableFunctions.insert(moveAssignment);
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
    Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1 || arity > 2) return;
    Cm::Sym::TypeSymbol* leftArgType = arguments[0].Type();
    if (leftArgType->IsReferenceType() || leftArgType->IsRvalueRefType() || !leftArgType->IsPointerToClassType()) return;
    Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(leftArgType->GetBaseType());
    SynthesizedClassFunGroupMapIt i = synthesizedClassFunGroupMap.find(groupName);
    if (i != synthesizedClassFunGroupMap.end())
    {
        SynthesizedClassFunGroup* group = i->second;
        group->CollectViableFunctions(cacheMap, classType, arity, arguments, span, containerScope, viableFunctions, exception);
    }
}

} } // namespace Cm::Bind
