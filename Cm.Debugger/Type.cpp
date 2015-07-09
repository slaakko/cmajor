/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Type.hpp>

namespace Cm { namespace Debugger {

TypeExpr::TypeExpr(const std::string& primaryTypeName_) : basicTypeExpr(false), primaryTypeName(primaryTypeName_)
{
}

TypeExpr* TypeExpr::Clone() const
{
    TypeExpr* clone = new TypeExpr(primaryTypeName);
    if (basicTypeExpr)
    {
        clone->basicTypeExpr = true;
    }
    clone->derivations = derivations;
    for (const std::unique_ptr<TypeExpr>& typeArgument : typeArguments)
    {
        clone->typeArguments.push_back(std::unique_ptr<TypeExpr>(typeArgument->Clone()));
    }
    return clone;
}

std::vector<std::unique_ptr<TypeExpr>> MakeTemplateArgumentList(const std::vector<TypeExpr*>& typeArgumentList)
{
    std::vector<std::unique_ptr<TypeExpr>> templateArgumentList;
    for (TypeExpr* typeExpr : typeArgumentList)
    {
        templateArgumentList.push_back(std::unique_ptr<TypeExpr>(typeExpr));
    }
    return templateArgumentList;
}

std::string TypeExpr::ToString() const
{
    std::string s = primaryTypeName;
    if (!typeArguments.empty())
    {
        s.append("<");
        bool first = true;
        for (const std::unique_ptr<TypeExpr>& typeArg : typeArguments)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                s.append(", ");
            }
            s.append(typeArg->ToString());
        }
        s.append(">");
    }
    if (derivations.NumDerivations() > 0)
    {
        std::vector<int> arrayDimensions;
        s = Cm::Ast::MakeDerivedTypeName(derivations, s, arrayDimensions);
    }
    return s;
}

} } // Cm::Debugger
