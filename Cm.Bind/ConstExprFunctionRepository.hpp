/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
#define CM_BIND_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED

#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/ConstExprFunctionRepository.hpp>


namespace Cm { namespace Bind {

class ConstExprFunctionRepository : public Cm::Core::ConstExprFunctionRepository
{
public:
    ConstExprFunctionRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void Release() override;
    Cm::Ast::Node* GetNodeFor(Cm::Sym::FunctionSymbol* constExprFunctionSymbol) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    std::unordered_set<Cm::Sym::FunctionSymbol*> constExprFunctions;
};

} } // namespace Cm::Bind

#endif // CM_BIND_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
