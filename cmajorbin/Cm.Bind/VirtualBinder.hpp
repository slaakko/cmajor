/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_VIRTUAL_BINDER_INCLUDED
#define CM_BIND_VIRTUAL_BINDER_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm {  namespace Bind {

class VirtualBinder : public Cm::Ast::Visitor
{
public:
    VirtualBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Ast::CompileUnitNode* compileUnit_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void EndVisit(Cm::Ast::ClassNode& classNode) override;
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Ast::CompileUnitNode* compileUnit;
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    void BindTemplateTypeSymbols(Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope);
};

} } // namespace Cm::Bind

#endif // CM_BIND_VIRTUAL_BINDER_INCLUDED


