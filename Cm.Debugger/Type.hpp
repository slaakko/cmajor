/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_TYPE_INCLUDED
#define CM_DEBUGGER_TYPE_INCLUDED
#include <Cm.Ast/TypeExpr.hpp>

namespace Cm { namespace Debugger {

class TypeExpr
{
public:
    TypeExpr(const std::string& primaryTypeName_);
    TypeExpr* Clone() const;
    const std::string& PrimaryTypeName() const { return primaryTypeName; }
    const Cm::Ast::DerivationList& Derivations() const { return derivations; }
    Cm::Ast::DerivationList& Derivations() { return derivations; }
    const std::vector<std::unique_ptr<TypeExpr>>& TypeArguments() const { return typeArguments; }
    std::vector<std::unique_ptr<TypeExpr>>& TypeArguments() { return typeArguments; }
    bool IsBasicTypeExpr() const { return basicTypeExpr; }
    void SetBasicTypeExpr() { basicTypeExpr = true; }
    std::string ToString() const;
private:
    bool basicTypeExpr;
    std::string primaryTypeName;
    Cm::Ast::DerivationList derivations;
    std::vector<std::unique_ptr<TypeExpr>> typeArguments;
};

std::vector<std::unique_ptr<TypeExpr>> MakeTemplateArgumentList(const std::vector<TypeExpr*>& typeArgumentList);

} } // Cm::Debugger

#endif // CM_DEBUGGER_TYPE_INCLUDED
