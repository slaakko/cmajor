/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/ClassSymbol.hpp>

namespace Cm { namespace Sym {

Symbol::Symbol(const Span& span_, const std::string& name_) : span(span_), name(name_), source(SymbolSource::project), parent(nullptr), bound(false)
{
}

Symbol::~Symbol()
{
}

void Symbol::Write(Writer& writer)
{
}

void Symbol::Read(Reader& reader)
{
}

std::string Symbol::FullName() const
{
    std::string parentFullName;
    if (parent)
    {
        parentFullName = parent->FullName();
    }
    if (parentFullName.empty())
    {
        return Name();
    }
    else
    {
        return parentFullName + "." + Name();
    }
}

NamespaceSymbol* Symbol::Ns() const
{
    if (IsNamespaceSymbol())
    {
        return const_cast<NamespaceSymbol*>(static_cast<const NamespaceSymbol*>(this));
    }
    else
    {
        if (parent)
        {
            return parent->Ns();
        }
        else
        {
            throw std::runtime_error("namespace not found");
        }
    }
}

ClassSymbol* Symbol::Class() const
{
    if (IsClassSymbol())
    {
        return const_cast<ClassSymbol*>(static_cast<const ClassSymbol*>(this));
    }
    else
    {
        if (parent)
        {
            return parent->Class();
        }
        else
        {
            throw std::runtime_error("class not found");
        }
    }
}

} } // namespace Cm::Sym
