/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
#define CM_CORE_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/Exception.hpp>

namespace Cm { namespace Core {

class ArrayTypeOpRepository
{
public:
    virtual ~ArrayTypeOpRepository();
    virtual void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Argument>& arguments, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
