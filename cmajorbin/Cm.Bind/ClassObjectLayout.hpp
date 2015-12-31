/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CLASS_OBJECT_LAYOUT_INCLUDED
#define CM_BIND_CLASS_OBJECT_LAYOUT_INCLUDED
#include <Cm.Bind/ExpressionBinder.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Bind {

void GenerateClassInitStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode, bool& callToThisInitializerGenerated);

void GenerateInitVPtrStatement(Cm::Sym::ClassTypeSymbol* classType, Cm::BoundTree::BoundFunction* boundFunction);

void GenerateMemberVariableInitStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::ConstructorNode* constructorNode);

void GenerateMemberVariableDestructionStatements(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::DestructorNode* destructorNode);

void GenerateBaseClassDestructionStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::DestructorNode* destructorNode);

void GenerateStaticInitStatement(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, Cm::Ast::StaticConstructorNode* staticConstructorNode);

void GenerateStaticConstructorCall(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* currentFunction, Cm::Sym::ClassTypeSymbol* classType, 
    Cm::Ast::MemberFunctionNode* memberFunctionNode);

} } // namespace Cm::Bind

#endif // CM_BIND_CLASS_OBJECT_LAYOUT_INCLUDED
