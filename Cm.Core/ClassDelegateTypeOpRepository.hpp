/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#define CM_CORE_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/ConversionTable.hpp>

namespace Cm { namespace Core {

class ClassDelegateTypeOpRepository
{
public:
    virtual ~ClassDelegateTypeOpRepository();
    virtual void CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
