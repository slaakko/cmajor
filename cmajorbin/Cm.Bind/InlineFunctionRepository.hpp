/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_INLINE_FUNCTION_REPOSITORY_INCLUDED
#define CM_BIND_INLINE_FUNCTION_REPOSITORY_INCLUDED
#include <Cm.Core/InlineFunctionRepository.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <unordered_set>

namespace Cm { namespace Bind {

class InlineFunctionRepository : public Cm::Core::InlineFunctionRepository
{
public:
    InlineFunctionRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    ~InlineFunctionRepository();
    void Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* functionSymbol) override;
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    std::unordered_set<Cm::Sym::FunctionSymbol*> instantiatedFunctions;
    std::vector<Cm::Sym::FunctionSymbol*> functionIntanceSymbols;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> ownedFunctionInstanceSymbols;
};

} } // namespace Cm::Bind

#endif // CM_BIND_INLINE_FUNCTION_REPOSITORY_INCLUDED
