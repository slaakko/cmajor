/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
#define CM_CORE_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Core {

class ConstExprFunctionRepository
{
public:
    virtual ~ConstExprFunctionRepository();
    virtual void Release() = 0;
    virtual Cm::Ast::Node* GetNodeFor(Cm::Sym::FunctionSymbol* constExprFunctionSymbol) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_CONST_EXPR_FUNCTION_REPOSITORY_INCLUDED
