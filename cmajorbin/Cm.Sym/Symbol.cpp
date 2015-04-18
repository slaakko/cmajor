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
#include <stdexcept>

namespace Cm { namespace Sym {
     
const char* symbolTypeStr[uint8_t(SymbolType::maxSymbol)] =
{
    "boolSymbol", "charSymbol", "voidSymbol", "sbyteSymbol", "byteSymbol", "shortSymbol", "ushortSymbol", "intSymbol", "uintSymbol", "longSymbol", "ulongSymbol", "floatSymbol", "doubleSymbol", "nullptrSymbol",
    "classSymbol", "constantSymbol", "declarationBlock", "delegateSymbol", "classDelegateSymbol", "enumTypeSymbol", "enumConstantSymbol", "functionSymbol", "functionGroupSymbol", "localVariableSymbol", "memberVariableSymbol",
    "namespaceSymbol", "parameterSymbol", "typeParameterSymbol", "templateTypeSymbol", "derivedTypeSymbol", "typedefSymbol", "boundTypeParameterSymbol"
};

std::string SymbolTypeStr(SymbolType st)
{
    return symbolTypeStr[uint8_t(st)];
}

const char* accessStr[4] =
{
    "private", "protected", "internal", "public"
};

std::string AccessStr(SymbolAccess access)
{
    return accessStr[uint8_t(access)];
}

std::string SymbolFlagStr(SymbolFlags flags, SymbolAccess declaredAccess)
{
    SymbolAccess access = SymbolAccess(flags & SymbolFlags::access);
    if (access != declaredAccess)
    {
        access = declaredAccess;
    }
    std::string s = AccessStr(access);
    if ((flags & SymbolFlags::bound) != SymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(" ");
        }
        s.append("bound");
    }
    if ((flags & SymbolFlags::static_) != SymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(" ");
        }
        s.append("static");
    }
    if ((flags & SymbolFlags::external) != SymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("external");
    }
    if ((flags & SymbolFlags::project) != SymbolFlags::none)
    {
        if (!s.empty())
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
    writer.GetBinaryWriter().Write(uint8_t(flags & ~(SymbolFlags::project | SymbolFlags::irTypeMade)));
}

void Symbol::Read(Reader& reader)
{
    flags = SymbolFlags(reader.GetBinaryReader().ReadByte());
}

std::string Symbol::FullName() const
{
    std::string parentFullName;
    Symbol* p = Parent();   // Parent(): derived type returns base type's parent if it has not parent of its own
    if (p)
    {
        parentFullName = p->FullName();
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

SymbolAccess Symbol::EffectiveAccess() const
{
    SymbolAccess effectiveAccess = Access();
    Symbol* p = Parent();
    while (p)
    {
        if (p->DeclaredAccess() < effectiveAccess)
        {
            effectiveAccess = p->DeclaredAccess();
        }
        p = p->Parent();
    }
    return effectiveAccess;
}

void Symbol::SetType(TypeSymbol* typeSymbol, int index)
{
    throw std::runtime_error("member function not applicable");
}

bool Symbol::IsExportSymbol() const 
{ 
    return Source() == SymbolSource::project && Access() == SymbolAccess::public_; 
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
    else if (that->parent)
    {
        return IsSameParentOrAncestorOf(that->parent);
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
            return nullptr;
        }
    }
}

void Symbol::Dump(CodeFormatter& formatter)
{
    std::string f = SymbolFlagStr(flags, DeclaredAccess());
    if (!f.empty())
    {
        f.append(1, ' ');
    }
    formatter.WriteLine(f + TypeString() + " " + Name());
}

void Symbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
}

void Symbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
}

void Symbol::InitVirtualFunctionTables()
{
}

void Symbol::MakeIrType()
{
    SetIrTypeMade();
}

} } // namespace Cm::Sym
