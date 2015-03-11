/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ClassTemplateRepository.hpp>
#include <Cm.Bind/Template.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void ClassTemplateRepository::BindTemplateTypeSymbol(Cm::Sym::TemplateTypeSymbol* templateTypeSymbol)
{
    Cm::Sym::TypeSymbol* subjectTypeSymbol = templateTypeSymbol->GetSubjectType();
    if (!subjectTypeSymbol->IsClassTypeSymbol())
    {
        throw std::runtime_error("subject type not class type");
    }
    Cm::Sym::ClassTypeSymbol* subjectClassTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(subjectTypeSymbol);
    classTemplates.insert(subjectClassTypeSymbol);
    Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(subjectTypeSymbol, false);
    if (!node)
    {
        subjectClassTypeSymbol->ReadClassNode(boundCompileUnit.SymbolTable(), subjectClassTypeSymbol->GetSpan().FileIndex());
        node = boundCompileUnit.SymbolTable().GetNode(subjectTypeSymbol);
    }
    if (!node->IsClassNode())
    {
        throw std::runtime_error("node is not class node");
    }
    Cm::Ast::ClassNode* classNode = static_cast<Cm::Ast::ClassNode*>(node);
    Cm::Ast::NamespaceNode* currentNs = nullptr;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs(CreateNamespaces(subjectClassTypeSymbol->GetSpan(), subjectClassTypeSymbol->Ns()->FullName(), subjectClassTypeSymbol->GetUsingNodes(), currentNs));
    Cm::Ast::CloneContext cloneContext;
    cloneContext.SetInstantiateClassNode();
    Cm::Ast::ClassNode* classInstanceNode = static_cast<Cm::Ast::ClassNode*>(classNode->Clone(cloneContext));
    Cm::Ast::IdentifierNode* classInstanceId = new Cm::Ast::IdentifierNode(classInstanceNode->GetSpan(), templateTypeSymbol->FullName());
    classInstanceNode->SetId(classInstanceId);
    currentNs->AddMember(classInstanceNode);
    int n = int(subjectClassTypeSymbol->TypeParameters().size());
    int m = int(templateTypeSymbol->TypeArguments().size());
    if (n < m)
    {
        throw Cm::Core::Exception("too many template arguments", templateTypeSymbol->GetSpan());
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::TypeParameterSymbol* typeParameterSymbol = subjectClassTypeSymbol->TypeParameters()[i];
        Cm::Sym::BoundTypeParameterSymbol* boundTypeParam = new Cm::Sym::BoundTypeParameterSymbol(typeParameterSymbol->GetSpan(), typeParameterSymbol->Name());
        Cm::Sym::TypeSymbol* typeArgument = nullptr;
        if (i < m)
        {
            typeArgument = templateTypeSymbol->TypeArguments()[i];
        }
        else
        {
            if (i >= classNode->TemplateParameters().Count())
            {
                throw Cm::Core::Exception("too few template arguments", templateTypeSymbol->GetSpan());
            }
            Cm::Ast::TemplateParameterNode* templateParameterNode = classNode->TemplateParameters()[i];
            Cm::Ast::Node* defaultTemplateArgumentNode = templateParameterNode->DefaultTemplateArgument();
            if (!defaultTemplateArgumentNode)
            {
                throw Cm::Core::Exception("too few template arguments", templateTypeSymbol->GetSpan());
            }
            typeArgument = ResolveType(boundCompileUnit.SymbolTable(), templateTypeSymbol->GetContainerScope(), boundCompileUnit.GetFileScopes(), *this, defaultTemplateArgumentNode);
        }
        boundTypeParam->SetType(typeArgument);
        templateTypeSymbol->AddSymbol(boundTypeParam);
    }
    Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
    declarationVisitor.SetTemplateType(classInstanceNode, templateTypeSymbol);
    globalNs->Accept(declarationVisitor);
    Prebinder prebinder(boundCompileUnit.SymbolTable(), *this);
    prebinder.BeginCompileUnit();
    globalNs->Accept(prebinder);
    prebinder.EndCompileUnit();
    Cm::Sym::FileScope* fileScope = prebinder.ReleaseFileScope();
    templateTypeSymbol->SetFileScope(fileScope);
    VirtualBinder virtualBinder(boundCompileUnit.SymbolTable(), boundCompileUnit.SyntaxUnit());
    globalNs->Accept(virtualBinder);
    Binder binder(boundCompileUnit);
    globalNs->Accept(binder);
    templateTypeSymbol->SetGlobalNs(globalNs.release());
}

ClassTemplateRepository::ClassTemplateRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

ClassTemplateRepository::~ClassTemplateRepository()
{
    try
    {
        for (Cm::Sym::ClassTypeSymbol* classTemplate : classTemplates)
        {
            classTemplate->FreeClassNode(boundCompileUnit.SymbolTable());
        }
    }
    catch (...)
    {
    }
}

void ClassTemplateRepository::CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span,
    Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions)
{
    if (int(arguments.size()) != arity)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    if (arity < 1) return;
    Cm::Sym::TypeSymbol* leftArgType = arguments[0].Type();
    if (leftArgType->IsReferenceType() || leftArgType->IsRvalueRefType() || !leftArgType->IsPointerToTemplateType()) return;
    Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(leftArgType->GetBaseType());
    if (!templateTypeSymbol->Bound())
    {
        BindTemplateTypeSymbol(templateTypeSymbol);
    }
}

void ClassTemplateRepository::Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* memberFunctionSymbol)
{
    if (boundCompileUnit.Instantiated(memberFunctionSymbol)) return;
    boundCompileUnit.AddToInstantiated(memberFunctionSymbol);
    memberFunctionSymbol->SetCompileUnit(boundCompileUnit.SyntaxUnit());
    Cm::Sym::Symbol* parent = memberFunctionSymbol->Parent();
    if (!parent->IsTemplateTypeSymbol())
    {
        throw std::runtime_error("not template type symbol");
    }
    Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(parent);
    Cm::Ast::Node* ttNode = boundCompileUnit.SymbolTable().GetNode(templateTypeSymbol);
    if (!ttNode->IsClassNode())
    {
        throw std::runtime_error("not class node");
    }
    Cm::Ast::ClassNode* templateTypeNode = static_cast<Cm::Ast::ClassNode*>(ttNode);
    Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(memberFunctionSymbol);
    if (!node->IsFunctionNode())
    {
        throw std::runtime_error("not function node");
    }
    Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
    bool isDefault = (functionNode->GetSpecifiers() & Cm::Ast::Specifiers::default_) != Cm::Ast::Specifiers::none;
    if (isDefault)
    {
        // implementation generated from elsewhere
    }
    else
    {
        Cm::Ast::CloneContext cloneContext;
        if (!functionNode->BodySource())
        {
            throw std::runtime_error("body source not set");
        }
        functionNode->SetBody(static_cast<Cm::Ast::CompoundStatementNode*>(functionNode->BodySource()->Clone(cloneContext)));
        functionNode->SetCompileUnit(boundCompileUnit.SyntaxUnit());
        boundCompileUnit.AddFileScope(templateTypeSymbol->CloneFileScope());

        Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
        boundCompileUnit.SymbolTable().BeginContainer(memberFunctionSymbol);
        functionNode->Body()->Accept(declarationVisitor);
        boundCompileUnit.SymbolTable().EndContainer();

        Prebinder prebinder(boundCompileUnit.SymbolTable(), *this);
        prebinder.SetDontCompleteFunctions();
        prebinder.SetCurrentClass(templateTypeSymbol);
        prebinder.BeginCompileUnit();
        Cm::Sym::ContainerScope* ttContainerScope = boundCompileUnit.SymbolTable().GetContainerScope(ttNode);
        prebinder.BeginContainerScope(ttContainerScope);
        functionNode->Accept(prebinder);
        prebinder.EndContainerScope();
        prebinder.EndCompileUnit();

        Binder binder(boundCompileUnit);
        binder.BeginVisit(*templateTypeNode);
        functionNode->Accept(binder);
        binder.EndVisit(*templateTypeNode);
        functionNode->SetBody(nullptr);
        boundCompileUnit.RemoveLastFileScope();
    }

    if (templateTypeSymbol->Destructor())
    {
        Instantiate(containerScope, templateTypeSymbol->Destructor());
    }
}

} } // namespace Cm::Bind
