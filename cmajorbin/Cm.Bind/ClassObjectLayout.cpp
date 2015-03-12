/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ClassObjectLayout.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Bind {

class ClassInitializerHandler : public ExpressionBinder
{
public:
    ClassInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_);
    void GenerateBaseInitializer(Cm::BoundTree::BoundExpressionList& arguments, const Cm::Parsing::Span& span, Cm::Ast::Node* baseInitializerNode);
    void Visit(Cm::Ast::BaseInitializerNode& baseInitializerNode);
    void Visit(Cm::Ast::ThisInitializerNode& thisInitializerNode);
    Cm::Ast::BaseInitializerNode* BaseInitializer() const { return baseInitializer; }
    Cm::Ast::ThisInitializerNode* ThisInitializer() const { return thisInitializer; }
private:
    Cm::Sym::ClassTypeSymbol* classType;
    Cm::Ast::BaseInitializerNode* baseInitializer;
    Cm::Ast::ThisInitializerNode* thisInitializer;
};

ClassInitializerHandler::ClassInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_) :
    ExpressionBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), classType(classType_), baseInitializer(nullptr), thisInitializer(nullptr)
{
}

void ClassInitializerHandler::GenerateBaseInitializer(Cm::BoundTree::BoundExpressionList& arguments, const Cm::Parsing::Span& span, Cm::Ast::Node* baseInitializerNode)
{
    Cm::Sym::ClassTypeSymbol* baseClassType = classType->BaseClass();
    BoundCompileUnit().IrClassTypeRepository().AddClassType(baseClassType);
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* baseClassPtrType = BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(baseClassType, span);
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
        baseClassCtor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, span, conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        throw Cm::Core::Exception("base class constructor not found: " + ex.Message(), ex.Defined(), ex.Referenced());
    }
    Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = BoundCompileUnit().ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, span);
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    arguments.InsertFront(thisAsBase); // insert 'this' to front
    PrepareFunctionArguments(baseClassCtor, ContainerScope(), BoundCompileUnit(), CurrentFunction(), arguments, false, BoundCompileUnit().IrClassTypeRepository());
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
            argument.reset(Cm::BoundTree::CreateBoundConversion(arg->SyntaxNode(), arg, conversionFun, CurrentFunction()));
        }
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(baseInitializerNode, std::move(arguments));
    functionCall->SetFunction(baseClassCtor);
    Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    int classObjectLayoutFunIndex = CurrentFunction()->GetClassObjectLayoutFunIndex();
    CurrentFunction()->Body()->InsertStatement(classObjectLayoutFunIndex, initBaseClasObjectStatement);
    ++classObjectLayoutFunIndex;
    CurrentFunction()->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

void ClassInitializerHandler::Visit(Cm::Ast::BaseInitializerNode& baseInitializerNode)
{
    if (baseInitializer)
    {
        throw Cm::Core::Exception("already has base class initializer", baseInitializerNode.GetSpan(), baseInitializer->GetSpan());
    }
    if (thisInitializer)
    {
        throw Cm::Core::Exception("cannot have both base class initializer and this class initializer", baseInitializerNode.GetSpan(), thisInitializer->GetSpan());
    }
    baseInitializer = &baseInitializerNode;
    Cm::Sym::ClassTypeSymbol* baseClassType = classType->BaseClass();
    if (!baseClassType)
    {
        throw Cm::Core::Exception("class does not have a base class", baseInitializerNode.GetSpan(), classType->GetSpan());
    }
    baseInitializerNode.Arguments().Accept(*this);
    Cm::BoundTree::BoundExpressionList arguments = GetExpressions();
    GenerateBaseInitializer(arguments, baseInitializerNode.GetSpan(), &baseInitializerNode);
}

void ClassInitializerHandler::Visit(Cm::Ast::ThisInitializerNode& thisInitializerNode)
{
    if (thisInitializer)
    {
        throw Cm::Core::Exception("already has this class initializer", thisInitializerNode.GetSpan(), thisInitializer->GetSpan());
    }
    if (baseInitializer)
    {
        throw Cm::Core::Exception("cannot have both this class initializer and base class initializer", thisInitializerNode.GetSpan(), baseInitializer->GetSpan());
    }
    thisInitializer = &thisInitializerNode;
    thisInitializerNode.Arguments().Accept(*this);
    Cm::BoundTree::BoundExpressionList arguments = GetExpressions();
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
    Cm::Core::Argument thisPointerArg(Cm::Core::ArgumentCategory::lvalue, thisParam->GetType());
    resolutionArguments.push_back(thisPointerArg);
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, classType->GetContainerScope()->ClassOrNsScope()));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* thisClassCtor = nullptr;
    try
    {
        thisClassCtor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, thisInitializerNode.GetSpan(), conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        throw Cm::Core::Exception("class constructor not found: " + ex.Message(), ex.Defined(), ex.Referenced());
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    arguments.InsertFront(boundThisParam); // insert 'this' to front
    PrepareFunctionArguments(thisClassCtor, ContainerScope(), BoundCompileUnit(), CurrentFunction(), arguments, false, BoundCompileUnit().IrClassTypeRepository());
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
            argument.reset(Cm::BoundTree::CreateBoundConversion(arg->SyntaxNode(), arg, conversionFun, CurrentFunction()));
        }
    }
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(&thisInitializerNode, std::move(arguments));
    functionCall->SetFunction(thisClassCtor);
    Cm::BoundTree::BoundInitClassObjectStatement* initClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    int classObjectLayoutFunIndex = CurrentFunction()->GetClassObjectLayoutFunIndex();
    CurrentFunction()->Body()->InsertStatement(classObjectLayoutFunIndex, initClasObjectStatement);
    ++classObjectLayoutFunIndex;
    CurrentFunction()->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

void GenerateClassInitStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode, bool& callToThisInitializerGenerated)
{
    callToThisInitializerGenerated = false;
    if (classType->StaticConstructor())
    {
        Cm::BoundTree::BoundExpressionList arguments;
        Cm::BoundTree::BoundFunctionCallStatement* staticConstructorCallStatement = new Cm::BoundTree::BoundFunctionCallStatement(classType->StaticConstructor(), std::move(arguments));
        staticConstructorCallStatement->SetTraceCallInfo(Cm::Bind::CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), constructorNode->GetSpan()));
        int classObjectLayoutFunIndex = currentFunction->GetClassObjectLayoutFunIndex();
        currentFunction->Body()->InsertStatement(classObjectLayoutFunIndex, staticConstructorCallStatement);
        ++classObjectLayoutFunIndex;
        currentFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
    }
    ClassInitializerHandler classInitializerHandler(boundCompileUnit, containerScope, fileScopes, currentFunction, classType);
    const Cm::Ast::InitializerNodeList& initializers = constructorNode->Initializers();
    for (const std::unique_ptr<Cm::Ast::InitializerNode>& initializerNode : initializers)
    {
        initializerNode->Accept(classInitializerHandler);
    }
    if (classInitializerHandler.ThisInitializer())
    {
        callToThisInitializerGenerated = true;
    }
    else if (!classInitializerHandler.BaseInitializer())
    {
        if (classType->BaseClass())
        {
            Cm::BoundTree::BoundExpressionList arguments;
            classInitializerHandler.GenerateBaseInitializer(arguments, constructorNode->GetSpan(), constructorNode);
        }
    }
}

void GenerateInitVPtrStatement(Cm::Sym::ClassTypeSymbol* classType, Cm::BoundTree::BoundFunction* boundFunction)
{
    if (!classType->IsVirtual()) return;
    int classObjectLayoutFunIndex = boundFunction->GetClassObjectLayoutFunIndex();
    boundFunction->Body()->InsertStatement(classObjectLayoutFunIndex, new Cm::BoundTree::BoundInitVPtrStatement(classType));
    ++classObjectLayoutFunIndex;
    boundFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

class MemberVariableInitializerHandler : public ExpressionBinder
{
public:
    MemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_);
    Cm::BoundTree::BoundInitMemberVariableStatement* GenerateMerberVariableInitializationStatement(Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments, 
        Cm::Ast::Node* node);
    void Visit(Cm::Ast::MemberInitializerNode& memberInitializerNode);
    void GenerateMemberVariableInitializationStatements(Cm::Ast::Node* constructorNode);
private:
    Cm::Sym::ClassTypeSymbol* classType;
    std::vector<std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>> initializationStatements;
    typedef std::unordered_map<std::string, int> MemberVariableNameIndexMap;
    typedef MemberVariableNameIndexMap::const_iterator MemberVariableNameIndexMapIt;
    MemberVariableNameIndexMap memberVariableNameIndexMap;
};

MemberVariableInitializerHandler::MemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_) : 
    ExpressionBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), classType(classType_)
{
    int n = int(classType->MemberVariables().size());
    initializationStatements.resize(n);
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
        memberVariableNameIndexMap[memberVariableSymbol->Name()] = i;
    }
}

Cm::BoundTree::BoundInitMemberVariableStatement* MemberVariableInitializerHandler::GenerateMerberVariableInitializationStatement(Cm::Sym::MemberVariableSymbol* memberVariableSymbol, 
    Cm::BoundTree::BoundExpressionList& arguments, Cm::Ast::Node* node)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(memberVariableType, node->GetSpan()));
    resolutionArguments.push_back(variableArgument);
    if (memberVariableType->IsPointerType() && arguments.Count() == 1 && arguments[0]->IsBoundNullLiteral())
    {
        arguments[0]->SetType(memberVariableType);
    }
    for (const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    {
        resolutionArguments.push_back(Cm::Core::Argument(argument->GetArgumentCategory(), argument->GetType()));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, memberVariableType->GetContainerScope()->ClassOrNsScope()));
    if (memberVariableType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
        BoundCompileUnit().IrClassTypeRepository().AddClassType(memberVarClassType);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberCtor = nullptr;
    try
    {
        memberCtor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, node->GetSpan(), conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        throw Cm::Core::Exception("constructor for member variable '" + memberVariableSymbol->Name() + "' not found: " + ex.Message(), ex.Defined(), ex.Referenced());
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(node, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    Cm::Sym::ParameterSymbol* thisParam = CurrentFunction()->GetFunctionSymbol()->Parameters()[0];
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    boundMemberVariable->SetClassObject(boundThisParam);
    arguments.InsertFront(boundMemberVariable);
    PrepareFunctionArguments(memberCtor, ContainerScope(), BoundCompileUnit(), CurrentFunction(), arguments, true, BoundCompileUnit().IrClassTypeRepository());
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
            argument.reset(Cm::BoundTree::CreateBoundConversion(arg->SyntaxNode(), arg, conversionFun, CurrentFunction()));
        }
    }
    Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = new Cm::BoundTree::BoundInitMemberVariableStatement(memberCtor, std::move(arguments));
    return initMemberVariableStatement;
}

void MemberVariableInitializerHandler::Visit(Cm::Ast::MemberInitializerNode& memberInitializerNode)
{
    const std::string& memberVariableName = memberInitializerNode.MemberVariableName();
    MemberVariableNameIndexMapIt i = memberVariableNameIndexMap.find(memberVariableName);
    if (i != memberVariableNameIndexMap.end())
    {
        int index = i->second;
        if (index < 0 || index >= int(initializationStatements.size()))
        {
            throw Cm::Core::Exception("invalid member variable initializer index for member variable '" + memberVariableName + "'", memberInitializerNode.GetSpan());
        }
        std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>& initStatement = initializationStatements[index];
        if (initStatement)
        {
            throw Cm::Core::Exception("member variable '" + memberVariableName + "' already has initializer", memberInitializerNode.GetSpan());
        }
        Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[index];
        memberInitializerNode.Arguments().Accept(*this);
        Cm::BoundTree::BoundExpressionList arguments = GetExpressions();
        initStatement.reset(GenerateMerberVariableInitializationStatement(memberVariableSymbol, arguments, &memberInitializerNode));
    }
    else
    {
        throw Cm::Core::Exception("member variable initializer '" + memberVariableName + "' not found", memberInitializerNode.GetSpan());
    }
}

void MemberVariableInitializerHandler::GenerateMemberVariableInitializationStatements(Cm::Ast::Node* constructorNode)
{
    int n = int(initializationStatements.size());
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>& initializationStatement = initializationStatements[i];
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = nullptr;
        if (initializationStatement)
        {
            initMemberVariableStatement = initializationStatement.release();
        }
        else
        {
            Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            Cm::BoundTree::BoundExpressionList arguments;
            initMemberVariableStatement = GenerateMerberVariableInitializationStatement(memberVariableSymbol, arguments, constructorNode);
        }
        int classObjectLayoutFunIndex = CurrentFunction()->GetClassObjectLayoutFunIndex();
        CurrentFunction()->Body()->InsertStatement(classObjectLayoutFunIndex, initMemberVariableStatement);
        ++classObjectLayoutFunIndex;
        CurrentFunction()->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
    }
}

void GenerateMemberVariableInitStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode)
{
    MemberVariableInitializerHandler memberVariableInitializerHandler(boundCompileUnit, containerScope, fileScopes, currentFunction, classType);
    const Cm::Ast::InitializerNodeList& initializers = constructorNode->Initializers();
    for (const std::unique_ptr<Cm::Ast::InitializerNode>& initializerNode : initializers)
    {
        initializerNode->Accept(memberVariableInitializerHandler);
    }
    memberVariableInitializerHandler.GenerateMemberVariableInitializationStatements(constructorNode);
}

void GenerateMemberVariableDestructionStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::DestructorNode* destructorNode)
{
    int n = int(classType->MemberVariables().size());
    for (int i = n - 1; i >= 0; --i)
    {
        Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
        Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
        if (!memberVariableType->IsClassTypeSymbol()) continue;
        Cm::Sym::ClassTypeSymbol* memberVariableClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableType);
        if (!memberVariableClassType->Destructor()) continue;
        Cm::Sym::FunctionSymbol* memberDtor = memberVariableClassType->Destructor();
        Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
        Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
        boundThisParam->SetType(thisParam->GetType());
        Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(destructorNode, memberVariableSymbol);
        boundMemberVariable->SetType(memberVariableSymbol->GetType());
        boundMemberVariable->SetClassObject(boundThisParam);
        Cm::BoundTree::BoundExpressionList arguments;
        arguments.Add(boundMemberVariable);
        PrepareFunctionArguments(memberDtor, containerScope, boundCompileUnit, currentFunction, arguments, true, boundCompileUnit.IrClassTypeRepository());
        Cm::BoundTree::BoundFunctionCallStatement* destroyMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberDtor, std::move(arguments));
        destroyMemberVariableStatement->SetTraceCallInfo(Cm::Bind::CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), destructorNode->GetSpan()));
        currentFunction->Body()->AddStatement(destroyMemberVariableStatement);
    }
}

void GenerateBaseClassDestructionStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::DestructorNode* destructorNode)
{
    if (!classType->BaseClass()) return;
    Cm::Sym::ClassTypeSymbol* baseClass = classType->BaseClass();
    if (!baseClass->Destructor()) return;
    Cm::Sym::FunctionSymbol* baseClassDtor = baseClass->Destructor();
    Cm::Sym::TypeSymbol* baseClassPtrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(baseClass, destructorNode->GetSpan());
    Cm::Sym::ParameterSymbol* thisParam = currentFunction->GetFunctionSymbol()->Parameters()[0];
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    Cm::Sym::FunctionSymbol* conversionFun = boundCompileUnit.ClassConversionTable().MakeBaseClassDerivedClassConversion(baseClassPtrType, thisParam->GetType(), 1, destructorNode->GetSpan());
    Cm::BoundTree::BoundConversion* thisAsBase = new Cm::BoundTree::BoundConversion(nullptr, boundThisParam, conversionFun);
    thisAsBase->SetType(baseClassPtrType);
    Cm::BoundTree::BoundExpressionList arguments;
    arguments.Add(thisAsBase); 
    PrepareFunctionArguments(baseClassDtor, containerScope, boundCompileUnit, currentFunction, arguments, true, boundCompileUnit.IrClassTypeRepository());
    Cm::BoundTree::BoundFunctionCallStatement* destroyBaseClassObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassDtor, std::move(arguments));
    destroyBaseClassObjectStatement->SetTraceCallInfo(Cm::Bind::CreateTraceCallInfo(boundCompileUnit, currentFunction->GetFunctionSymbol(), destructorNode->GetSpan()));
    currentFunction->Body()->AddStatement(destroyBaseClassObjectStatement);
}

void GenerateStaticCheckInitializedStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundFunction* currentFunction, 
    Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::Node* staticConstructorNode)
{
    Cm::Sym::MemberVariableSymbol* initializedVar = new Cm::Sym::MemberVariableSymbol(staticConstructorNode->GetSpan(), Cm::IrIntf::GetPrivateSeparator() + "initialized");
    initializedVar->SetParent(classType);
    Cm::Sym::TypeSymbol* boolType = boundCompileUnit.SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    initializedVar->SetType(boolType);
    initializedVar->SetStatic();
    classType->SetInitializedVar(initializedVar);
    Cm::BoundTree::BoundConditionalStatement* checkInitializedStatement = new Cm::BoundTree::BoundConditionalStatement(staticConstructorNode);
    Cm::BoundTree::BoundMemberVariable* boundInitializedVar = new Cm::BoundTree::BoundMemberVariable(staticConstructorNode, initializedVar);
    boundInitializedVar->SetFlag(Cm::BoundTree::BoundNodeFlags::genJumpingBoolCode);
    checkInitializedStatement->SetCondition(boundInitializedVar);
    Cm::BoundTree::BoundReturnStatement* returnStatement = new Cm::BoundTree::BoundReturnStatement(staticConstructorNode);
    checkInitializedStatement->AddStatement(returnStatement);
    int classObjectLayoutFunIndex = currentFunction->GetClassObjectLayoutFunIndex();
    currentFunction->Body()->InsertStatement(classObjectLayoutFunIndex, checkInitializedStatement);
    ++classObjectLayoutFunIndex;
    currentFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
    Cm::BoundTree::BoundMemberVariable* boundInitializedVarLeft = new Cm::BoundTree::BoundMemberVariable(staticConstructorNode, initializedVar);
    boundInitializedVarLeft->SetFlag(Cm::BoundTree::BoundNodeFlags::lvalue);
    Cm::BoundTree::BoundLiteral* boundTrue = new Cm::BoundTree::BoundLiteral(staticConstructorNode);
    boundTrue->SetValue(new Cm::Sym::BoolValue(true));
    boundTrue->SetType(boolType);
    std::vector<Cm::Sym::FunctionSymbol*> boolAssignConversions;
    std::vector<Cm::Core::Argument> boolAssignArgs;
    boolAssignArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(boolType, staticConstructorNode->GetSpan())));
    boolAssignArgs.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, boolType));
    Cm::Sym::FunctionLookupSet boolAssignLookups;
    boolAssignLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_parent, containerScope));
    Cm::Sym::FunctionSymbol* boolAssignment = ResolveOverload(containerScope, boundCompileUnit, "operator=", boolAssignArgs, boolAssignLookups, staticConstructorNode->GetSpan(), 
        boolAssignConversions);
    Cm::BoundTree::BoundAssignmentStatement* setInitializedStatement = new Cm::BoundTree::BoundAssignmentStatement(staticConstructorNode, boundInitializedVarLeft, boundTrue, boolAssignment);
    int classObjectLayoutFunIndex1 = currentFunction->GetClassObjectLayoutFunIndex();
    currentFunction->Body()->InsertStatement(classObjectLayoutFunIndex1, setInitializedStatement);
    ++classObjectLayoutFunIndex1;
    currentFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex1);
}

void GenerateStaticBaseClassInitStatement(Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::Node* staticConstructorNode)
{
    Cm::BoundTree::BoundExpressionList arguments;
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(staticConstructorNode, std::move(arguments));
    functionCall->SetFunction(classType->BaseClass()->StaticConstructor());
    Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    int classObjectLayoutFunIndex = currentFunction->GetClassObjectLayoutFunIndex();
    currentFunction->Body()->InsertStatement(classObjectLayoutFunIndex, initBaseClasObjectStatement);
    ++classObjectLayoutFunIndex;
    currentFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

class StaticMemberVariableInitializerHandler : public ExpressionBinder
{
public:
    StaticMemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_);
    Cm::BoundTree::BoundInitMemberVariableStatement* GenerateStaticMerberVariableInitializationStatement(Cm::Sym::MemberVariableSymbol* memberVariableSymbol, Cm::BoundTree::BoundExpressionList& arguments,
        Cm::Ast::Node* node);
    void Visit(Cm::Ast::MemberInitializerNode& memberInitializerNode);
    void GenerateStaticMemberVariableInitializationStatements(Cm::Ast::Node* staticConstructorNode);
private:
    Cm::Sym::ClassTypeSymbol* classType;
    std::vector<std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>> initializationStatements;
    typedef std::unordered_map<std::string, int> StaticMemberVariableNameIndexMap;
    typedef StaticMemberVariableNameIndexMap::const_iterator StaticMemberVariableNameIndexMapIt;
    StaticMemberVariableNameIndexMap staticMemberVariableNameIndexMap;
};

StaticMemberVariableInitializerHandler::StaticMemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_) : 
    ExpressionBinder(boundCompileUnit_, containerScope_, fileScopes_, currentFunction_), classType(classType_)
{
    int n = int(classType->StaticMemberVariables().size());
    initializationStatements.resize(n);
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->StaticMemberVariables()[i];
        staticMemberVariableNameIndexMap[memberVariableSymbol->Name()] = i;
    }
}

Cm::BoundTree::BoundInitMemberVariableStatement* StaticMemberVariableInitializerHandler::GenerateStaticMerberVariableInitializationStatement(Cm::Sym::MemberVariableSymbol* memberVariableSymbol,
    Cm::BoundTree::BoundExpressionList& arguments, Cm::Ast::Node* node)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
    Cm::Core::Argument variableArgument(Cm::Core::ArgumentCategory::lvalue, BoundCompileUnit().SymbolTable().GetTypeRepository().MakePointerType(memberVariableType, node->GetSpan()));
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
        BoundCompileUnit().IrClassTypeRepository().AddClassType(memberVarClassType);
    }
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* memberCtor = nullptr;
    try
    {
        memberCtor = ResolveOverload(ContainerScope(), BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, node->GetSpan(), conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        throw Cm::Core::Exception("constructor for member variable '" + memberVariableSymbol->Name() + "' not found: " + ex.Message(), ex.Defined(), ex.Referenced());
    }
    Cm::BoundTree::BoundMemberVariable* boundMemberVariable = new Cm::BoundTree::BoundMemberVariable(node, memberVariableSymbol);
    boundMemberVariable->SetType(memberVariableSymbol->GetType());
    arguments.InsertFront(boundMemberVariable);
    PrepareFunctionArguments(memberCtor, ContainerScope(), BoundCompileUnit(), CurrentFunction(), arguments, true, BoundCompileUnit().IrClassTypeRepository());
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
            argument.reset(Cm::BoundTree::CreateBoundConversion(arg->SyntaxNode(), arg, conversionFun, CurrentFunction()));
        }
    }
    Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = new Cm::BoundTree::BoundInitMemberVariableStatement(memberCtor, std::move(arguments));
    return initMemberVariableStatement;
}

void StaticMemberVariableInitializerHandler::Visit(Cm::Ast::MemberInitializerNode& memberInitializerNode)
{
    const std::string& memberVariableName = memberInitializerNode.MemberVariableName();
    StaticMemberVariableNameIndexMapIt i = staticMemberVariableNameIndexMap.find(memberVariableName);
    if (i != staticMemberVariableNameIndexMap.end())
    {
        int index = i->second;
        if (index < 0 || index >= int(initializationStatements.size()))
        {
            throw Cm::Core::Exception("invalid static member variable initializer index for static member variable '" + memberVariableName + "'", memberInitializerNode.GetSpan());
        }
        std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>& initStatement = initializationStatements[index];
        if (initStatement)
        {
            throw Cm::Core::Exception("static member variable '" + memberVariableName + "' already has initializer", memberInitializerNode.GetSpan());
        }
        Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol = classType->StaticMemberVariables()[index];
        memberInitializerNode.Arguments().Accept(*this);
        Cm::BoundTree::BoundExpressionList arguments = GetExpressions();
        initStatement.reset(GenerateStaticMerberVariableInitializationStatement(staticMemberVariableSymbol, arguments, &memberInitializerNode));
        if (staticMemberVariableSymbol->GetType()->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(staticMemberVariableSymbol->GetType());
            if (memberVarClassType->Destructor())
            {
                initStatement->SetRegisterDestructor();
                initStatement->SetMemberVariableSymbol(staticMemberVariableSymbol);
            }
        }
    }
    else
    {
        throw Cm::Core::Exception("member variable initializer '" + memberVariableName + "' not found", memberInitializerNode.GetSpan());
    }
}

void StaticMemberVariableInitializerHandler::GenerateStaticMemberVariableInitializationStatements(Cm::Ast::Node* staticConstructorNode)
{
    int n = int(initializationStatements.size());
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::BoundTree::BoundInitMemberVariableStatement>& initializationStatement = initializationStatements[i];
        Cm::BoundTree::BoundInitMemberVariableStatement* initMemberVariableStatement = nullptr;
        if (initializationStatement)
        {
            initMemberVariableStatement = initializationStatement.release();
        }
        else
        {
            Cm::Sym::MemberVariableSymbol* memberVariableSymbol = classType->StaticMemberVariables()[i];
            Cm::BoundTree::BoundExpressionList arguments;
            initMemberVariableStatement = GenerateStaticMerberVariableInitializationStatement(memberVariableSymbol, arguments, staticConstructorNode);
            if (memberVariableSymbol->GetType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableSymbol->GetType());
                if (memberVarClassType->Destructor())
                {
                    initMemberVariableStatement->SetRegisterDestructor();
                    initMemberVariableStatement->SetMemberVariableSymbol(memberVariableSymbol);
                }
            }
        }
        int classObjectLayoutFunIndex = CurrentFunction()->GetClassObjectLayoutFunIndex();
        CurrentFunction()->Body()->InsertStatement(classObjectLayoutFunIndex, initMemberVariableStatement);
        ++classObjectLayoutFunIndex;
        CurrentFunction()->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
    }
}

void GenerateStaticInitStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::StaticConstructorNode* staticConstructorNode)
{
    GenerateStaticCheckInitializedStatement(boundCompileUnit, containerScope, currentFunction, classType, staticConstructorNode);
    if (classType->BaseClass() && classType->BaseClass()->StaticConstructor())
    {
        GenerateStaticBaseClassInitStatement(currentFunction, classType, staticConstructorNode);
    }
    StaticMemberVariableInitializerHandler staticMemberVariableInitializerHandler(boundCompileUnit, containerScope, fileScopes, currentFunction, classType);
    const Cm::Ast::InitializerNodeList& initializers = staticConstructorNode->Initializers();
    for (const std::unique_ptr<Cm::Ast::InitializerNode>& initializerNode : initializers)
    {
        initializerNode->Accept(staticMemberVariableInitializerHandler);
    }
    staticMemberVariableInitializerHandler.GenerateStaticMemberVariableInitializationStatements(staticConstructorNode);
}

void GenerateStaticConstructorCall(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, 
    Cm::Ast::MemberFunctionNode* memberFunctionNode)
{
    Cm::BoundTree::BoundExpressionList arguments;
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(memberFunctionNode, std::move(arguments));
    functionCall->SetFunction(classType->StaticConstructor());
    Cm::BoundTree::BoundInitClassObjectStatement* initBaseClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    int classObjectLayoutFunIndex = currentFunction->GetClassObjectLayoutFunIndex();
    currentFunction->Body()->InsertStatement(classObjectLayoutFunIndex, initBaseClasObjectStatement);
    ++classObjectLayoutFunIndex;
    currentFunction->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

} } // namespace Cm::Bind
