/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_SYNTHESIZED_CLASS_FUN_REPOSITORY_INCLUDED
#define CM_CORE_SYNTHESIZED_CLASS_FUN_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/Exception.hpp>

namespace Cm { namespace Core {

// implementation provided by Cm::Bind::SynthesizedClassFunRepository

class SynthesizedClassFunRepository
{
public:
    virtual ~SynthesizedClassFunRepository();
    virtual void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, Exception*& exception) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_SYNTHESIZED_CLASS_FUN_REPOSITORY_INCLUDED
