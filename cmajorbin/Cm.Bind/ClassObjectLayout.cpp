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
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

class ClassInitializerHandler : public ExpressionBinder
{
public:
    ClassInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundFunction* currentFunction_,
        Cm::Sym::ClassTypeSymbol* classType_);
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

ClassInitializerHandler::ClassInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, 
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_) :
    ExpressionBinder(boundCompileUnit_, containerScope_, 
    fileScope_, currentFunction_), classType(classType_), baseInitializer(nullptr), thisInitializer(nullptr)
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
        baseClassCtor = ResolveOverload(BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, span, conversions);
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
    PrepareFunctionArguments(baseClassCtor, arguments, false, BoundCompileUnit().IrClassTypeRepository());
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
        thisClassCtor = ResolveOverload(BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, thisInitializerNode.GetSpan(), conversions);
    }
    catch (const Cm::Core::Exception& ex)
    {
        throw Cm::Core::Exception("class constructor not found: " + ex.Message(), ex.Defined(), ex.Referenced());
    }
    Cm::BoundTree::BoundParameter* boundThisParam = new Cm::BoundTree::BoundParameter(nullptr, thisParam);
    boundThisParam->SetType(thisParam->GetType());
    arguments.InsertFront(boundThisParam); // insert 'this' to front
    PrepareFunctionArguments(thisClassCtor, arguments, false, BoundCompileUnit().IrClassTypeRepository());
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
    Cm::BoundTree::BoundFunctionCall* functionCall = new Cm::BoundTree::BoundFunctionCall(&thisInitializerNode, std::move(arguments));
    functionCall->SetFunction(thisClassCtor);
    Cm::BoundTree::BoundInitClassObjectStatement* initClasObjectStatement = new Cm::BoundTree::BoundInitClassObjectStatement(functionCall);
    int classObjectLayoutFunIndex = CurrentFunction()->GetClassObjectLayoutFunIndex();
    CurrentFunction()->Body()->InsertStatement(classObjectLayoutFunIndex, initClasObjectStatement);
    ++classObjectLayoutFunIndex;
    CurrentFunction()->SetClassObjectLayoutFunIndex(classObjectLayoutFunIndex);
}

void GenerateClassInitStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::BoundTree::BoundFunction* currentFunction, 
    Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode, bool& callToThisInitializerGenerated)
{
    callToThisInitializerGenerated = false;
    ClassInitializerHandler classInitializerHandler(boundCompileUnit, containerScope, fileScope, currentFunction, classType);
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
    MemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, 
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

MemberVariableInitializerHandler::MemberVariableInitializerHandler(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, 
    Cm::BoundTree::BoundFunction* currentFunction_, Cm::Sym::ClassTypeSymbol* classType_) : ExpressionBinder(boundCompileUnit_, containerScope_, fileScope_, currentFunction_), classType(classType_)
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
        memberCtor = ResolveOverload(BoundCompileUnit(), "@constructor", resolutionArguments, functionLookups, node->GetSpan(), conversions);
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
    PrepareFunctionArguments(memberCtor, arguments, true, BoundCompileUnit().IrClassTypeRepository());
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

void GenerateMemberVariableInitStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, 
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode)
{
    MemberVariableInitializerHandler memberVariableInitializerHandler(boundCompileUnit, containerScope, fileScope, currentFunction, classType);
    const Cm::Ast::InitializerNodeList& initializers = constructorNode->Initializers();
    for (const std::unique_ptr<Cm::Ast::InitializerNode>& initializerNode : initializers)
    {
        initializerNode->Accept(memberVariableInitializerHandler);
    }
    memberVariableInitializerHandler.GenerateMemberVariableInitializationStatements(constructorNode);
}

void GenerateMemberVariableDestructionStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, 
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::DestructorNode* destructorNode)
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
        PrepareFunctionArguments(memberDtor, arguments, true, boundCompileUnit.IrClassTypeRepository());
        Cm::BoundTree::BoundFunctionCallStatement* destroyMemberVariableStatement = new Cm::BoundTree::BoundFunctionCallStatement(memberDtor, std::move(arguments));
        currentFunction->Body()->AddStatement(destroyMemberVariableStatement);
    }
}

void GenerateBaseClassDestructionStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, 
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
    PrepareFunctionArguments(baseClassDtor, arguments, true, boundCompileUnit.IrClassTypeRepository());
    Cm::BoundTree::BoundFunctionCallStatement* destroyBaseClassObjectStatement = new Cm::BoundTree::BoundFunctionCallStatement(baseClassDtor, std::move(arguments));
    currentFunction->Body()->AddStatement(destroyBaseClassObjectStatement);
}

} } // namespace Cm::Bind
