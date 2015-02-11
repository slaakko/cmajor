/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Concept.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Sym/ConceptGroupSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/IntrinsicConcept.hpp>

namespace Cm { namespace Bind {

class ConstraintCheckStack
{
public:
    void Push(bool value)
    {
        stack.push(value);
    }
    bool Pop()
    {
        if (stack.empty())
        {
            throw std::runtime_error("constraint check stack is empty");
        }
        bool top = stack.top();
        stack.pop();
        return top;
    }
private:
    std::stack<bool> stack;
};

Cm::Sym::InstantiatedConceptSymbol* Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit* boundCompileUnit, Cm::Sym::FileScope* functionFileScope, 
    Cm::Sym::ConceptSymbol* conceptSymbol, const std::vector<Cm::Sym::TypeSymbol*>& typeArguments);

class ConstraintChecker : public Cm::Ast::Visitor
{
public:
    ConstraintChecker(Cm::Sym::TypeSymbol* firstTypeArgument_, Cm::Sym::TypeSymbol* secondTypeArgument_, Cm::Sym::ContainerScope* containerScope_, Cm::BoundTree::BoundCompileUnit* boundCompileUnit_, 
        Cm::Sym::FileScope* functionFileScope_);
    bool GetResult() { return constraintCheckStack.Pop(); }
    void Visit(Cm::Ast::ConceptNode& conceptNode) override;
    void Visit(Cm::Ast::ConceptIdNode& conceptIdNode) override;
    void Visit(Cm::Ast::DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(Cm::Ast::ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(Cm::Ast::IsConstraintNode& isConstraintNode) override;
    void Visit(Cm::Ast::MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(Cm::Ast::TypenameConstraintNode& typenameConstraintNode) override;
    void Visit(Cm::Ast::ConstructorConstraintNode& constructorConstraintNode) override;
    void Visit(Cm::Ast::DestructorConstraintNode& destructorConstraintNode) override;
    void Visit(Cm::Ast::MemberFunctionConstraintNode& memberFunctionConstraintNode) override;
    void Visit(Cm::Ast::FunctionConstraintNode& functionConstraintNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void Visit(Cm::Ast::BoolNode& boolNode) override;
    void Visit(Cm::Ast::SByteNode& sbyteNode) override;
    void Visit(Cm::Ast::ByteNode& byteNode) override;
    void Visit(Cm::Ast::ShortNode& shortNode) override;
    void Visit(Cm::Ast::UShortNode& shortNode) override;
    void Visit(Cm::Ast::IntNode& intNode) override;
    void Visit(Cm::Ast::UIntNode& uintNode) override;
    void Visit(Cm::Ast::LongNode& longNode) override;
    void Visit(Cm::Ast::ULongNode& ulongNode) override;
    void Visit(Cm::Ast::FloatNode& floatNode) override;
    void Visit(Cm::Ast::DoubleNode& doubleNode) override;
    void Visit(Cm::Ast::CharNode& charNode) override;
    void Visit(Cm::Ast::VoidNode& voidNode) override;
    void Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode) override;
    void Visit(Cm::Ast::SameConstraintNode& sameConstraintNode) override;
    void Visit(Cm::Ast::DerivedConstraintNode& derivedConstraintNode) override;
    void Visit(Cm::Ast::ConvertibleConstraintNode& convertibleConstraintNode) override;
    void Visit(Cm::Ast::ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) override;
private:
    Cm::Sym::TypeSymbol* firstTypeArgument;
    Cm::Sym::TypeSymbol* secondTypeArgument;
    Cm::Sym::ContainerScope* containerScope;
    Cm::BoundTree::BoundCompileUnit* boundCompileUnit;
    Cm::Sym::FileScope* functionFileScope;
    ConstraintCheckStack constraintCheckStack;
    Cm::Sym::TypeSymbol* type;
    Cm::Sym::ConceptGroupSymbol* conceptGroup;
};

ConstraintChecker::ConstraintChecker(Cm::Sym::TypeSymbol* firstTypeArgument_, Cm::Sym::TypeSymbol* secondTypeArgument_, Cm::Sym::ContainerScope* containerScope_, 
    Cm::BoundTree::BoundCompileUnit* boundCompileUnit_, Cm::Sym::FileScope* functionFileScope_) :
    Cm::Ast::Visitor(false, true), containerScope(containerScope_), firstTypeArgument(firstTypeArgument_), secondTypeArgument(secondTypeArgument_), boundCompileUnit(boundCompileUnit_), 
    functionFileScope(functionFileScope_), type(nullptr)
{
}

void ConstraintChecker::Visit(Cm::Ast::ConceptNode& conceptNode)
{
    if (conceptNode.Refinement())
    {
        conceptNode.Refinement()->Accept(*this);
        bool satisfied = constraintCheckStack.Pop();
        if (!satisfied)
        {
            constraintCheckStack.Push(false);
            return;
        }
    }
    for (const std::unique_ptr<Cm::Ast::ConstraintNode>& constraint : conceptNode.Constraints())
    {
        constraint->Accept(*this);
        bool satisfied = constraintCheckStack.Pop();
        if (!satisfied)
        {
            constraintCheckStack.Push(false);
            return;
        }
    }
    constraintCheckStack.Push(true);
}

void ConstraintChecker::Visit(Cm::Ast::ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    if (conceptGroup)
    {
        int n = conceptIdNode.TypeParameters().Count();
        Cm::Sym::ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        std::vector<Cm::Sym::TypeSymbol*> typeArguments;
        for (int i = 0; i < n; ++i)
        {
            Cm::Ast::Node* typeParamNode = conceptIdNode.TypeParameters()[i];
            typeParamNode->Accept(*this);
            if (type)
            {
                typeArguments.push_back(type);
            }
        }
        Cm::Sym::TypeId conceptId = Cm::Sym::ComputeInstantiatedConceptSymbolTypeId(conceptSymbol, typeArguments);
        Cm::Sym::InstantiatedConceptSymbol* instantiatedConcept = boundCompileUnit->ConceptRepository().GetInstantiatedConcept(conceptId);
        if (instantiatedConcept)
        {
            constraintCheckStack.Push(true);
        }
        else
        {
            instantiatedConcept = Instantiate(containerScope, boundCompileUnit, functionFileScope, conceptSymbol, typeArguments);
            if (instantiatedConcept)
            {
                boundCompileUnit->ConceptRepository().AddInstantiatedConcept(conceptId, instantiatedConcept);
                constraintCheckStack.Push(true);
            }
            else
            {
                constraintCheckStack.Push(false);
            }
        }
    }
    else
    {
        throw Cm::Core::ConceptCheckException("concept group expected", conceptIdNode.GetSpan());
    }
}

void ConstraintChecker::Visit(Cm::Ast::DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    bool left = false;
    try
    {
        disjunctiveConstraintNode.Left()->Accept(*this);
        left = constraintCheckStack.Pop();
        if (left)
        {
            constraintCheckStack.Push(true);
            return;
        }
    }
    catch (...)
    {
    }
    disjunctiveConstraintNode.Right()->Accept(*this);
    bool right = constraintCheckStack.Pop();
    constraintCheckStack.Push(right);
}

void ConstraintChecker::Visit(Cm::Ast::ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    conjunctiveConstraintNode.Left()->Accept(*this);
    bool left = constraintCheckStack.Pop();
    if (!left)
    {
        constraintCheckStack.Push(false);
    }
    conjunctiveConstraintNode.Right()->Accept(*this);
    bool right = constraintCheckStack.Pop();
    constraintCheckStack.Push(right);
}

void ConstraintChecker::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    type = nullptr;
    conceptGroup = nullptr;
    Cm::Sym::Symbol* symbol = containerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        symbol = boundCompileUnit->GetFileScope()->Lookup(identifierNode.Str());
    }
    if (!symbol)
    {
        symbol = functionFileScope->Lookup(identifierNode.Str());
    }
    if (symbol)
    {
        if (symbol->IsBoundTypeParameterSymbol())
        {
            Cm::Sym::BoundTypeParameterSymbol* boundTypeParameterSymbol = static_cast<Cm::Sym::BoundTypeParameterSymbol*>(symbol);
            type = boundTypeParameterSymbol->GetType();
        }
        else if (symbol->IsConceptGroupSymbol())
        {
            conceptGroup = static_cast<Cm::Sym::ConceptGroupSymbol*>(symbol);
        }
        else
        {
            throw Cm::Core::ConceptCheckException("symbol '" + symbol->FullName() + "' does not denote a type or a concept", symbol->GetSpan());
        }
    }
    else
    {
        throw Cm::Core::ConceptCheckException("symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
}

void ConstraintChecker::Visit(Cm::Ast::BoolNode& boolNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
}

void ConstraintChecker::Visit(Cm::Ast::SByteNode& sbyteNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
}

void ConstraintChecker::Visit(Cm::Ast::ByteNode& byteNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
}

void ConstraintChecker::Visit(Cm::Ast::ShortNode& shortNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
}

void ConstraintChecker::Visit(Cm::Ast::UShortNode& shortNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
}

void ConstraintChecker::Visit(Cm::Ast::IntNode& intNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
}

void ConstraintChecker::Visit(Cm::Ast::UIntNode& uintNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
}

void ConstraintChecker::Visit(Cm::Ast::LongNode& longNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
}

void ConstraintChecker::Visit(Cm::Ast::ULongNode& ulongNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
}

void ConstraintChecker::Visit(Cm::Ast::FloatNode& floatNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
}

void ConstraintChecker::Visit(Cm::Ast::DoubleNode& doubleNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
}

void ConstraintChecker::Visit(Cm::Ast::CharNode& charNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
}

void ConstraintChecker::Visit(Cm::Ast::VoidNode& voidNode)
{
    type = boundCompileUnit->SymbolTable().GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
}

void ConstraintChecker::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    type = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), &derivedTypeExprNode);
}

void ConstraintChecker::Visit(Cm::Ast::IsConstraintNode& isConstraintNode)
{
    Cm::Ast::Node* typeExpr = isConstraintNode.TypeExpr();
    typeExpr->Accept(*this);
    Cm::Sym::TypeSymbol* leftType = type;
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    if (type)
    {
        Cm::Sym::TypeSymbol* rightType = type;
        Cm::Sym::TypeSymbol* leftPlainType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePlainType(leftType);
        Cm::Sym::TypeSymbol* rightPlainType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePlainType(rightType);
        if (Cm::Sym::TypesEqual(leftPlainType, rightPlainType))
        {
            constraintCheckStack.Push(true);
        }
        else
        {
            constraintCheckStack.Push(false);
        }
    }
    else if (conceptGroup)
    {
        Cm::Sym::ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(1);
        std::vector<Cm::Sym::TypeSymbol*> typeArguments;
        typeArguments.push_back(leftType);
        Cm::Sym::TypeId conceptId = Cm::Sym::ComputeInstantiatedConceptSymbolTypeId(conceptSymbol, typeArguments);
        Cm::Sym::InstantiatedConceptSymbol* instantiatedConcept = boundCompileUnit->ConceptRepository().GetInstantiatedConcept(conceptId);
        if (instantiatedConcept)
        {
            constraintCheckStack.Push(true);
        }
        else
        {
            instantiatedConcept = Instantiate(containerScope, boundCompileUnit, functionFileScope, conceptSymbol, typeArguments);
            if (instantiatedConcept)
            {
                boundCompileUnit->ConceptRepository().AddInstantiatedConcept(conceptId, instantiatedConcept);
                constraintCheckStack.Push(true);
            }
            else
            {
                constraintCheckStack.Push(false);
            }
        }
    }
}

void ConstraintChecker::Visit(Cm::Ast::MultiParamConstraintNode& multiParamConstraintNode)
{
    multiParamConstraintNode.ConceptId()->Accept(*this);
    if (conceptGroup)
    {
        int n = multiParamConstraintNode.TypeExprNodes().Count();
        Cm::Sym::ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        std::vector<Cm::Sym::TypeSymbol*> typeArguments;
        for (int i = 0; i < n; ++i)
        {
            Cm::Ast::Node* typeExprNode = multiParamConstraintNode.TypeExprNodes()[i];
            typeExprNode->Accept(*this);
            if (type)
            {
                typeArguments.push_back(type);
            }
            else
            {
                throw Cm::Core::ConceptCheckException("'" + typeExprNode->Name() + "' is not bound to a type", typeExprNode->GetSpan());
            }
        }
        Cm::Sym::TypeId conceptId = Cm::Sym::ComputeInstantiatedConceptSymbolTypeId(conceptSymbol, typeArguments);
        Cm::Sym::InstantiatedConceptSymbol* instantiatedConcept = boundCompileUnit->ConceptRepository().GetInstantiatedConcept(conceptId);
        if (instantiatedConcept)
        {
            constraintCheckStack.Push(true);
        }
        instantiatedConcept = Instantiate(containerScope, boundCompileUnit, functionFileScope, conceptSymbol, typeArguments);
        if (instantiatedConcept)
        {
            boundCompileUnit->ConceptRepository().AddInstantiatedConcept(conceptId, instantiatedConcept);
            constraintCheckStack.Push(true);
        }
        else
        {
            constraintCheckStack.Push(false);
        }
    }
    else
    {
        throw Cm::Core::ConceptCheckException("'" + multiParamConstraintNode.ConceptId()->Str() + "' does not denote a concept group", multiParamConstraintNode.ConceptId()->GetSpan());
    }
}

void ConstraintChecker::Visit(Cm::Ast::TypenameConstraintNode& typenameConstraintNode)
{
    typenameConstraintNode.TypeId()->Accept(*this);
    constraintCheckStack.Push(type != nullptr);
}

void ConstraintChecker::Visit(Cm::Ast::ConstructorConstraintNode& constructorConstraintNode)
{
    std::vector<Cm::Core::Argument> resolutionArguments;
    std::vector<Cm::Sym::TypeSymbol*> parameterTypes;
    Cm::Sym::TypeSymbol* thisParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePointerType(firstTypeArgument, constructorConstraintNode.GetSpan());
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::ParameterNode* parameterNode = constructorConstraintNode.Parameters()[i];
        Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), parameterNode->TypeExpr());
        Cm::Core::ArgumentCategory argumentCategory = parameterType->IsReferenceType() ? Cm::Core::ArgumentCategory::lvalue : Cm::Core::ArgumentCategory::rvalue;
        resolutionArguments.push_back(Cm::Core::Argument(argumentCategory, parameterType));
        parameterTypes.push_back(parameterType);
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, firstTypeArgument->GetContainerScope()->ClassOrNsScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, "@constructor", resolutionArguments, functionLookups, constructorConstraintNode.GetSpan(), conversions,
        OverloadResolutionFlags::nothrow);
    if (!functionSymbol)
    {
        std::string signature;
        signature.append(firstTypeArgument->FullName()).append(1, '.').append(firstTypeArgument->Name()).append(1, '(');
        bool first = true;
        for (Cm::Sym::TypeSymbol* parameterType : parameterTypes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                signature.append(", ");
            }
            signature.append(parameterType->FullName());
        }
        signature.append(1, ')');
        throw Cm::Core::ConceptCheckException("constructor signature '" + signature + "' not found");
    }
    else
    {
        constraintCheckStack.Push(true);
    }
}

void ConstraintChecker::Visit(Cm::Ast::DestructorConstraintNode& destructorConstraintNode)
{
    constraintCheckStack.Push(true);
}

void ConstraintChecker::Visit(Cm::Ast::MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    if (!type)
    {
        throw Cm::Core::ConceptCheckException("type parameter '" + memberFunctionConstraintNode.TypeParamId()->Str() + "' is not bound to type", memberFunctionConstraintNode.TypeParamId()->GetSpan());
    }
    const std::string& groupName = memberFunctionConstraintNode.GroupId()->Str();
    std::vector<Cm::Sym::TypeSymbol*> parameterTypes;
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::TypeSymbol* thisParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePointerType(type, memberFunctionConstraintNode.GetSpan());
    resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        Cm::Ast::ParameterNode* parameterNode = memberFunctionConstraintNode.Parameters()[i];
        Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), parameterNode->TypeExpr());
        Cm::Core::ArgumentCategory argumentCategory = parameterType->IsReferenceType() ? Cm::Core::ArgumentCategory::lvalue : Cm::Core::ArgumentCategory::rvalue;
        resolutionArguments.push_back(Cm::Core::Argument(argumentCategory, parameterType));
        parameterTypes.push_back(parameterType);
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, type->GetContainerScope()->ClassOrNsScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, groupName, resolutionArguments, functionLookups, memberFunctionConstraintNode.GetSpan(), conversions,
        OverloadResolutionFlags::nothrow);
    if (!functionSymbol)
    {
        std::string signature;
        signature.append(type->FullName()).append(1, '.').append(groupName).append(1, '(');
        bool first = true;
        for (Cm::Sym::TypeSymbol* parameterType : parameterTypes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                signature.append(", ");
            }
            signature.append(parameterType->FullName());
        }
        signature.append(1, ')');
        throw Cm::Core::ConceptCheckException("member function signature '" + signature + "' not found");
    }
    else
    {
        constraintCheckStack.Push(true);
    }
}

void ConstraintChecker::Visit(Cm::Ast::FunctionConstraintNode& functionConstraintNode)
{
    const std::string& groupName = functionConstraintNode.GroupId()->Str();
    std::vector<Cm::Core::Argument> resolutionArguments;
    Cm::Sym::FunctionLookupSet functionLookups;
    Cm::Sym::FunctionSymbol* functionSymbol = nullptr;
    if (Cm::Core::IsMemberFunctionGroup(groupName))
    {
        Cm::Sym::TypeSymbol* thisParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePointerType(firstTypeArgument, functionConstraintNode.GetSpan());
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, firstTypeArgument->GetContainerScope()->ClassOrNsScope()));
        int n = functionConstraintNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            Cm::Ast::ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
            Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), parameterNode->TypeExpr());
            Cm::Core::ArgumentCategory argumentCategory = parameterType->IsReferenceType() ? Cm::Core::ArgumentCategory::lvalue : Cm::Core::ArgumentCategory::rvalue;
            resolutionArguments.push_back(Cm::Core::Argument(argumentCategory, parameterType));
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, parameterType->GetContainerScope()->ClassOrNsScope()));
        }
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, groupName, resolutionArguments, functionLookups, functionConstraintNode.GetSpan(), conversions, 
            OverloadResolutionFlags::nothrow);
        if (!functionSymbol)
        {
            resolutionArguments.clear();
            resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
            for (int i = 1; i < n; ++i)
            {
                Cm::Ast::ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
                Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), parameterNode->TypeExpr());
                Cm::Core::ArgumentCategory argumentCategory = parameterType->IsReferenceType() ? Cm::Core::ArgumentCategory::lvalue : Cm::Core::ArgumentCategory::rvalue;
                resolutionArguments.push_back(Cm::Core::Argument(argumentCategory, parameterType));
            }
            std::vector<Cm::Sym::FunctionSymbol*> conversions;
            functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, groupName, resolutionArguments, functionLookups, functionConstraintNode.GetSpan(), conversions,
                OverloadResolutionFlags::nothrow);
        }
    }
    std::vector<Cm::Sym::TypeSymbol*> parameterTypes;
    if (!functionSymbol)
    {
        resolutionArguments.clear();
        functionLookups.Clear();
        int n = functionConstraintNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            Cm::Ast::ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
            Cm::Sym::TypeSymbol* parameterType = ResolveType(boundCompileUnit->SymbolTable(), containerScope, boundCompileUnit->GetFileScope(), parameterNode->TypeExpr());
            Cm::Core::ArgumentCategory argumentCategory = parameterType->IsReferenceType() ? Cm::Core::ArgumentCategory::lvalue : Cm::Core::ArgumentCategory::rvalue;
            resolutionArguments.push_back(Cm::Core::Argument(argumentCategory, parameterType));
            parameterTypes.push_back(parameterType);
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, parameterType->GetContainerScope()->ClassOrNsScope()));
        }
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, containerScope));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, groupName, resolutionArguments, functionLookups, functionConstraintNode.GetSpan(), conversions,
            OverloadResolutionFlags::nothrow);
    }
    if (!functionSymbol)
    {
        std::string signature;
        signature.append(groupName).append(1, '(');
        bool first = true;
        for (Cm::Sym::TypeSymbol* parameterType : parameterTypes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                signature.append(", ");
            }
            signature.append(parameterType->FullName());
        }
        signature.append(1, ')');
        throw Cm::Core::ConceptCheckException("function signature '" + signature + "' not found");
    }
    else
    {
        constraintCheckStack.Push(true);
    }
}

void ConstraintChecker::Visit(Cm::Ast::SameConstraintNode& sameConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        bool same = Cm::Sym::TypesEqual(firstTypeArgument, secondTypeArgument);
        if (!same)
        {
            throw Cm::Core::ConceptCheckException("type '" + firstTypeArgument->FullName() + "' is not same as type '" + secondTypeArgument->FullName() + "'");
        }
        else
        {
            constraintCheckStack.Push(true);
        }
    }
    else
    {
        throw Cm::Core::ConceptCheckException("Intrinsic 'Same' concept needs two type arguments");
    }
}

void ConstraintChecker::Visit(Cm::Ast::DerivedConstraintNode& derivedConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        bool derived = false;
        if (firstTypeArgument->IsClassTypeSymbol() && secondTypeArgument->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* firstClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(firstTypeArgument);
            Cm::Sym::ClassTypeSymbol* secondClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(secondTypeArgument);
            derived = firstClassType->HasBaseClass(secondClassType);
        }
        if (!derived)
        {
            throw Cm::Core::ConceptCheckException("type '" + firstTypeArgument->FullName() + "' is not derived from type '" + secondTypeArgument->FullName() + "'");
        }
        else
        {
            constraintCheckStack.Push(true);
        }
    }
    else
    {
        throw Cm::Core::ConceptCheckException("Intrinsic 'Derived' concept needs two type arguments");
    }
}

void ConstraintChecker::Visit(Cm::Ast::ConvertibleConstraintNode& convertibleConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        Cm::Sym::TypeSymbol* thisParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePointerType(secondTypeArgument, convertibleConstraintNode.GetSpan());
        Cm::Sym::TypeSymbol* thatParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakeConstReferenceType(firstTypeArgument, convertibleConstraintNode.GetSpan());
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, secondTypeArgument->GetContainerScope()->ClassOrNsScope()));
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thatParameterType));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, "@constructor", resolutionArguments, functionLookups, convertibleConstraintNode.GetSpan(), conversions,
            Cm::Sym::ConversionType::implicit, OverloadResolutionFlags::nothrow);
        if (!functionSymbol)
        {
            throw Cm::Core::ConceptCheckException("type '" + firstTypeArgument->FullName() + "' is not convertible to type '" + secondTypeArgument->FullName() + "'");
        }
        constraintCheckStack.Push(true);
    }
    else
    {
        throw Cm::Core::ConceptCheckException("Intrinsic 'Convertible' concept needs two type arguments");
    }
}

void ConstraintChecker::Visit(Cm::Ast::ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        Cm::Sym::TypeSymbol* thisParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakePointerType(secondTypeArgument, explicitlyConvertibleConstraintNode.GetSpan());
        Cm::Sym::TypeSymbol* thatParameterType = boundCompileUnit->SymbolTable().GetTypeRepository().MakeConstReferenceType(firstTypeArgument, explicitlyConvertibleConstraintNode.GetSpan());
        std::vector<Cm::Core::Argument> resolutionArguments;
        Cm::Sym::FunctionLookupSet functionLookups;
        functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_, secondTypeArgument->GetContainerScope()->ClassOrNsScope()));
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thisParameterType));
        resolutionArguments.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, thatParameterType));
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* functionSymbol = ResolveOverload(containerScope, *boundCompileUnit, "@constructor", resolutionArguments, functionLookups, explicitlyConvertibleConstraintNode.GetSpan(), conversions,
            Cm::Sym::ConversionType::explicit_, OverloadResolutionFlags::nothrow);
        if (!functionSymbol)
        {
            throw Cm::Core::ConceptCheckException("type '" + firstTypeArgument->FullName() + "' is not explicitly convertible to type '" + secondTypeArgument->FullName() + "'");
        }
        constraintCheckStack.Push(true);
    }
    else
    {
        throw Cm::Core::ConceptCheckException("Intrinsic 'ExplicitlyConvertible' concept needs two type arguments");
    }
}

Cm::Sym::InstantiatedConceptSymbol* Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit* boundCompileUnit, Cm::Sym::FileScope* functionFileScope, 
    Cm::Sym::ConceptSymbol* conceptSymbol, const std::vector<Cm::Sym::TypeSymbol*>& typeArguments)
{
    Cm::Ast::Node* node = boundCompileUnit->SymbolTable().GetNode(conceptSymbol);
    if (node && node->IsConceptNode())
    {
        Cm::Ast::ConceptNode* conceptNode = static_cast<Cm::Ast::ConceptNode*>(node);
        int n = int(conceptSymbol->TypeParameters().size());
        if (n != int(typeArguments.size()))
        {
            throw std::runtime_error("wrong number of type arguments");
        }
        Cm::Sym::ContainerScope instantiationScope;
        instantiationScope.SetParent(containerScope);
        std::vector<std::unique_ptr<Cm::Sym::BoundTypeParameterSymbol>> boundTypeParameters;
        Cm::Sym::TypeSymbol* firstTypeArgument = nullptr;
        Cm::Sym::TypeSymbol* secondTypeArgument = nullptr;
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::TypeParameterSymbol* typeParameterSymbol = conceptSymbol->TypeParameters()[i];
            Cm::Sym::TypeSymbol* typeArgument = typeArguments[i];
            if (i == 0)
            {
                firstTypeArgument = typeArgument;
            }
            else if (i == 1)
            {
                secondTypeArgument = typeArgument;
            }
            Cm::Sym::BoundTypeParameterSymbol* boundTypeParameterSymbol = new Cm::Sym::BoundTypeParameterSymbol(conceptSymbol->GetSpan(), typeParameterSymbol->Name());
            boundTypeParameterSymbol->SetType(typeArgument);
            boundTypeParameters.push_back(std::unique_ptr<Cm::Sym::BoundTypeParameterSymbol>(boundTypeParameterSymbol));
            instantiationScope.Install(boundTypeParameterSymbol);
        }
        ConstraintChecker checker(firstTypeArgument, secondTypeArgument, &instantiationScope, boundCompileUnit, functionFileScope);
        try
        {
            conceptNode->Accept(checker);
        }
        catch (const Cm::Core::ConceptCheckException& ex)
        {
            std::string message;
            if (typeArguments.size() == 1)
            {
                message.append("type '" + firstTypeArgument->FullName() + "' does not fulfill the requirements of concept ");
            }
            else
            {
                message.append("types (");
                bool first = true;
                for (Cm::Sym::TypeSymbol* typeArgument : typeArguments)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        message.append(", ");
                    }
                    message.append("'" + typeArgument->FullName() + "'");
                }
                message.append(") do not fulfill the requirements of concept ");
            }
            message.append(conceptSymbol->FullName()).append(" because:\n");
            message.append(ex.Message());
            throw Cm::Core::ConceptCheckException(message, ex.Defined());
        }
        bool result = checker.GetResult();
        if (result)
        {
            Cm::Sym::InstantiatedConceptSymbol* instantiatedConceptSymbol = new Cm::Sym::InstantiatedConceptSymbol(conceptSymbol, typeArguments);
            return instantiatedConceptSymbol;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        throw std::runtime_error("concept node not found in symbol table");
    }
    return nullptr;
}

bool CheckConstraint(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FileScope* functionFileScope, Cm::Ast::WhereConstraintNode* constraint,
    const std::vector<Cm::Sym::TypeParameterSymbol*>& templateParameters, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments, Cm::Core::ConceptCheckException& exception)
{
    bool result = false;
    try
    {
        Cm::Sym::ContainerScope constraintCheckScope;
        constraintCheckScope.SetParent(containerScope);
        int n = int(templateParameters.size());
        if (n != int(templateArguments.size()))
        {
            throw std::runtime_error("wrong number of template arguments");
        }
        std::vector<std::unique_ptr<Cm::Sym::BoundTypeParameterSymbol>> boundTypeParameters;
        Cm::Sym::TypeSymbol* firstTypeArgument = nullptr;
        Cm::Sym::TypeSymbol* secondTypeArgument = nullptr;
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::TypeParameterSymbol* templateParameter = templateParameters[i];
            Cm::Sym::TypeSymbol* templateArgument = templateArguments[i];
            if (i == 0)
            {
                firstTypeArgument = templateArgument;
            }
            else if (i == 1)
            {
                secondTypeArgument = templateArgument;
            }
            Cm::Sym::BoundTypeParameterSymbol* boundTypeParameter = new Cm::Sym::BoundTypeParameterSymbol(templateParameter->GetSpan(), templateParameter->Name());
            boundTypeParameters.push_back(std::unique_ptr<Cm::Sym::BoundTypeParameterSymbol>(boundTypeParameter));
            boundTypeParameter->SetType(templateArgument);
            constraintCheckScope.Install(boundTypeParameter);
        }
        ConstraintChecker constraintChecker(firstTypeArgument, secondTypeArgument, &constraintCheckScope, &boundCompileUnit, functionFileScope);
        constraint->Accept(constraintChecker);
        result = constraintChecker.GetResult();
    }
    catch (Cm::Core::ConceptCheckException& ex)
    {
        exception = ex;
        result = false;
    }
    return result;
}

} } // namespace Cm::Bind
