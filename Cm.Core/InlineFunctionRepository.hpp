/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_INLINE_FUNCTION_REPOSITORY_INCLUDED
#define CM_CORE_INLINE_FUNCTION_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/Exception.hpp>

namespace Cm { namespace Core {

// implementation provided by Cm::Bind::InlineFunctionRepository

class InlineFunctionRepository
{
public:
    virtual ~InlineFunctionRepository();
    virtual void Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* functionSymbol) = 0;
    virtual void Write(Cm::Sym::BcuWriter& writer) = 0;
    virtual void Read(Cm::Sym::BcuReader& reader) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_INLINE_FUNCTION_REPOSITORY_INCLUDED
