/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

const char* symbolTypeStr[uint8_t(SymbolType::maxSymbol)] =
{
    "boolSymbol", "charSymbol", "voidSymbol", "sbyteSymbol", "byteSymbol", "shortSymbol", "ushortSymbol", "intSymbol", "uintSymbol", "longSymbol", "ulongSymbol", "floatSymbol", "doubleSymbol",
    "classSymbol", "constantSymbol", "declarationBlock", "delegateSymbol", "classDelegateSymbol", "enumTypeSymbol", "enumConstantSymbol", "functionSymbol", "localVariableSymbol", "memberVariableSymbol",
    "namespaceSymbol", "parameterSymbol", "templateParameterSymbol", "templateTypeSymbol", "typeSymbol", "derivedTypeSymbol", "typedefSymbol"
};

std::string SymbolTypeStr(SymbolType st)
{
    return symbolTypeStr[uint8_t(st)];
}

std::string SymbolFlagStr(SymbolFlags flags)
{
    std::string s;
    if ((flags & SymbolFlags::public_) != SymbolFlags::none)
    {
        s.append("public");
    }
    if ((flags & SymbolFlags::protected_) != SymbolFlags::none)
    {
        if (s.empty())
        {
            s.append(" ");
        }
        s.append("protected");
    }
    if ((flags & SymbolFlags::private_) != SymbolFlags::none)
    {
        if (s.empty())
        {
            s.append(" ");
        }
        s.append("private");
    }
    if ((flags & SymbolFlags::internal_) != SymbolFlags::none)
    {
        if (s.empty())
        {
            s.append(" ");
        }
        s.append("internal");
    }
    if ((flags & SymbolFlags::export_) != SymbolFlags::none)
    {
        if (s.empty())
        {
            s.append(" ");
        }
        s.append("export");
    }
    if ((flags & SymbolFlags::projectSource) != SymbolFlags::none)
    {
        if (s.empty())
        {
            s.append(" ");
        }
        s.append("project");
    }
    return s;
}

Symbol::Symbol(const Span& span_, const std::string& name_) : span(span_), name(name_), flags(), parent(nullptr)
{
    SetSource(SymbolSource::project);
}

Symbol::~Symbol()
{
}

void Symbol::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(uint8_t(flags & ~(SymbolFlags::projectSource | SymbolFlags::export_)));
}

void Symbol::Read(Reader& reader)
{
    flags = SymbolFlags(reader.GetBinaryReader().ReadByte());
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

void Symbol::SetType(TypeSymbol* typeSymbol, int index)
{
    throw std::runtime_error("member function not applicable");
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

ClassTypeSymbol* Symbol::Class() const
{
    if (IsClassSymbol())
    {
        return const_cast<ClassTypeSymbol*>(static_cast<const ClassTypeSymbol*>(this));
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

void Symbol::Dump(CodeFormatter& formatter)
{
    std::string f = SymbolFlagStr(flags);
    if (!f.empty())
    {
        f.append(1, ' ');
    }
    formatter.WriteLine(f + TypeString() + " " + Name());
}

} } // namespace Cm::Sym
