/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/InlineFunctionRepository.hpp>
#include <Cm.Bind/Template.hpp>
#include <Cm.Bind/Prebinder.hpp>
#include <Cm.Bind/Binder.hpp>
#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Sym/DeclarationVisitor.hpp>

namespace Cm { namespace Bind {

InlineFunctionRepository::InlineFunctionRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

InlineFunctionRepository::~InlineFunctionRepository()
{
    try
    {
        for (Cm::Sym::FunctionSymbol* inlineFunction : instantiatedFunctions)
        {
            inlineFunction->FreeFunctionNode(boundCompileUnit.SymbolTable());
        }
    }
    catch (...)
    {
    }
}

void InlineFunctionRepository::Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* functionSymbol)
{
    if (functionSymbol->CompileUnit() == boundCompileUnit.SyntaxUnit()) return;
    if (instantiatedFunctions.find(functionSymbol) != instantiatedFunctions.end()) return;
    instantiatedFunctions.insert(functionSymbol);
    Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(functionSymbol, false);
    if (!node)
    {
        functionSymbol->ReadFunctionNode(boundCompileUnit.SymbolTable(), functionSymbol->GetSpan().FileIndex());
        node = boundCompileUnit.SymbolTable().GetNode(functionSymbol);
    }
    if (!node->IsFunctionNode())
    {
        throw std::runtime_error("node is not function node");
    }
    Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
    Cm::Ast::NamespaceNode* currentNs = nullptr;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs(CreateNamespaces(functionSymbol->GetSpan(), functionSymbol->Ns()->FullName(), functionSymbol->GetUsingNodes(), currentNs));
    Cm::Ast::CloneContext cloneContext;
    Cm::Ast::FunctionNode* functionInstanceNode = static_cast<Cm::Ast::FunctionNode*>(functionNode->Clone(cloneContext));
    currentNs->AddMember(functionInstanceNode);
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = nullptr;
    if (functionSymbol->IsMemberFunctionSymbol())
    {
        classTypeSymbol = functionSymbol->Class();
        boundCompileUnit.SymbolTable().SetCurrentClass(classTypeSymbol);
    }
    Cm::Sym::DeclarationVisitor declarationVisitor(boundCompileUnit.SymbolTable());
    globalNs->Accept(declarationVisitor);
    Prebinder prebinder(boundCompileUnit.SymbolTable(), boundCompileUnit.ClassTemplateRepository());
    if (classTypeSymbol)
    {
        prebinder.SetCurrentClass(classTypeSymbol);
    }
    prebinder.BeginCompileUnit();
    globalNs->Accept(prebinder);
    prebinder.EndCompileUnit();
    Cm::Sym::FileScope* fileScope = prebinder.ReleaseFileScope();
    boundCompileUnit.AddFileScope(fileScope);
    Cm::Sym::FunctionSymbol* functionInstanceSymbol = boundCompileUnit.SymbolTable().GetFunctionSymbol(functionInstanceNode);
    functionInstanceSymbol->SetReplica();
    if (classTypeSymbol)
    {
        functionInstanceSymbol->SetParent(classTypeSymbol);
        functionInstanceSymbol->GetContainerScope()->SetParent(classTypeSymbol->GetContainerScope());
    }
    Binder binder(boundCompileUnit);
    if (classTypeSymbol)
    {
        binder.BeginClass(classTypeSymbol);
    }
    globalNs->Accept(binder);
    if (classTypeSymbol)
    {
        binder.EndClass();
    }
    boundCompileUnit.RemoveLastFileScope();
    functionSymbol->SetGlobalNs(globalNs.release());
}

} } // namespace Cm::Bind
