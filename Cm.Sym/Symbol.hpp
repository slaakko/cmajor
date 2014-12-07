/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_INCLUDED
#define CM_SYM_SYMBOL_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Sym {

class ContainerScope;
class NamespaceSymbol;
class ClassSymbol;

class Symbol
{
public:
    Symbol();
    Symbol(const std::string& name_);
    virtual ~Symbol();
    const std::string& Name() const { return name; }
    std::string FullName() const;
    Symbol* Parent() const { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    Cm::Ast::Node* GetNode() const { return node; }
    void SetNode(Cm::Ast::Node* node_) { node = node_; }
    virtual ContainerScope* GetContainerScope() { return nullptr; }
    virtual bool IsNamespaceSymbol() const { return false; }
    virtual bool IsClassSymbol() const { return false; }
    NamespaceSymbol* Ns() const;
    ClassSymbol* Class() const;
private:
    std::string name;
    Symbol* parent;
    Cm::Ast::Node* node;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_INCLUDED
