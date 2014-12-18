/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

const char* symbolTypeStr[uint8_t(SymbolType::maxSymbol)] =
{
    "boolSymbol", "charSymbol", "voidSymbol", "sbyteSymbol", "byteSymbol", "shortSymbol", "ushortSymbol", "intSymbol", "uintSymbol", "longSymbol", "ulongSymbol", "floatSymbol", "doubleSymbol",
    "classSymbol", "constantSymbol", "declarationBlock", "delegateSymbol", "classDelegateSymbol", "enumTypeSymbol", "enumConstantSymbol", "functionSymbol", "functionGroupSymbol", "localVariableSymbol", "memberVariableSymbol",
    "namespaceSymbol", "parameterSymbol", "templateParameterSymbol", "templateTypeSymbol", "typeSymbol", "derivedTypeSymbol", "typedefSymbol"
};

std::string SymbolTypeStr(SymbolType st)
{
    return symbolTypeStr[uint8_t(st)];
}

const char* accessStr[4] =
{
    "private", "protected", "internal", "public"
};

std::string Accesstr(SymbolAccess access)
{
    return accessStr[uint8_t(access)];
}

std::string SymbolFlagStr(SymbolFlags flags)
{
    std::string s = Accesstr(SymbolAccess(flags & SymbolFlags::access));
    if ((flags & SymbolFlags::project) != SymbolFlags::none)
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
    writer.GetBinaryWriter().Write(uint8_t(flags & ~(SymbolFlags::project)));
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

bool Symbol::IsExportSymbol() const 
{ 
    return Source() == SymbolSource::project && Access() == SymbolAccess::public_; 
}

bool Symbol::WillBeExported() const
{
    return IsExportSymbol() && (!parent || parent && parent->WillBeExported());
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
    if (IsClassTypeSymbol())
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
            return nullptr;
        }
    }
}

ClassTypeSymbol* Symbol::ContainingClass() const
{
    if (parent)
    { 
        return parent->Class();
    }
    else
    {
        return nullptr;
    }
}

FunctionSymbol* Symbol::Function() const
{
    if (IsFunctionSymbol())
    {
        return const_cast<FunctionSymbol*>(static_cast<const FunctionSymbol*>(this));
    }
    else
    {
        if (parent)
        {
            return parent->Function();
        }
        else
        {
            return nullptr;
        }
    }
}

FunctionSymbol* Symbol::ContainingFunction() const
{
    if (parent)
    {
        return parent->Function();
    }
    else
    {
        return nullptr;
    }
}

bool Symbol::IsSameParentOrAncestorOf(Symbol* that) const
{
    if (!that)
    {
        return false;
    }
    if (this == that)
    {
        return true;
    }
    else if (parent)
    {
        return parent->IsSameParentOrAncestorOf(that);
    }
    else
    {
        return false;
    }
}

ContainerSymbol* Symbol::ClassOrNs() const
{
    if (IsClassTypeSymbol() || IsNamespaceSymbol())
    {
        return const_cast<ContainerSymbol*>(static_cast<const ContainerSymbol*>(this));
    }
    else
    {
        if (parent)
        {
            return parent->ClassOrNs();
        }
        else
        {
            throw std::runtime_error("containing class or namespace symbol not found");
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
