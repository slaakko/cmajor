/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EVALUATOR_INCLUDED
#define CM_BIND_EVALUATOR_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/Scope.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <stack>
#include <memory>

namespace Cm { namespace Bind {

enum class EvaluationFlags : uint8_t
{
    none = 0, dontThrow = 1
};

inline EvaluationFlags operator&(EvaluationFlags left, EvaluationFlags right)
{
    return EvaluationFlags(uint8_t(left) & uint8_t(right));
}

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    EvaluationFlags flags);

bool IsAlwaysTrue(Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);

} } // namespace Cm::Bind

#endif // CM_BIND_EVALUATOR_INCLUDED
