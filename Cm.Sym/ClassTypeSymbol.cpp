/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_, TypeId()), baseClass(nullptr)
{
}

std::string ClassTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

void ClassTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
}

void ClassTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
}

bool ClassTypeSymbol::HasBaseClass(ClassTypeSymbol* cls) const
{
    return baseClass == cls || baseClass && baseClass->HasBaseClass(cls);
}

bool ClassTypeSymbol::HasBaseClass(ClassTypeSymbol* cls, int& distance) const
{
    if (!baseClass) return false;
    ++distance;
    if (baseClass == cls) return true;
    return baseClass->HasBaseClass(cls, distance);

}

void ClassTypeSymbol::AddSymbol(Symbol* symbol)
{
    TypeSymbol::AddSymbol(symbol);
    if (symbol->IsMemberVariableSymbol())
    {
        MemberVariableSymbol* memberVariableSymbol = static_cast<MemberVariableSymbol*>(symbol);
        memberVariables.push_back(memberVariableSymbol);
    }
}

} } // namespace Cm::Sym
