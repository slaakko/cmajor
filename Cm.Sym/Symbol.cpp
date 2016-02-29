/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Symbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/InterfaceTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <stdexcept>
#include <iostream>
#include <unordered_set>

namespace Cm { namespace Sym {
     
const char* symbolTypeStr[uint8_t(SymbolType::maxSymbol)] =
{
    "boolSymbol", "charSymbol", "wcharSymbol", "ucharSymbol", "voidSymbol", "sbyteSymbol", "byteSymbol", "shortSymbol", "ushortSymbol", "intSymbol", "uintSymbol", "longSymbol", "ulongSymbol", 
    "floatSymbol", "doubleSymbol", "nullptrSymbol",
    "classSymbol", "constantSymbol", "declarationBlock", "delegateSymbol", "classDelegateSymbol", "enumTypeSymbol", "enumConstantSymbol", "functionSymbol", "functionGroupSymbol", "localVariableSymbol", "memberVariableSymbol",
    "namespaceSymbol", "parameterSymbol", "typeParameterSymbol", "templateTypeSymbol", "derivedTypeSymbol", "typedefSymbol", "boundTypeParameterSymbol", "conceptSymbol", "conceptGroupSymbol", 
    "instantiatedConceptSymbol", "functionGroupTypeSymbol", "entrySymbol", "returnValueSymbol", "interfaceSymbol"
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

std::string SymbolFlagStr(SymbolFlags flags, SymbolAccess declaredAccess, bool addAccess)
{
    SymbolAccess access = SymbolAccess(flags & SymbolFlags::access);
    if (access != declaredAccess)
    {
        access = declaredAccess;
    }
    std::string s;
    if (addAccess)
    {
        s.append(AccessStr(access));
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
    return s;
}

SymbolCollector::~SymbolCollector()
{
}

Symbol::Symbol(const Span& span_, const std::string& name_) : sid(noSid), span(span_), name(name_), flags(), parent(nullptr)
{
    SetSource(SymbolSource::project);
}

void Symbol::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(sid);
    writer.GetBinaryWriter().Write(uint16_t(flags & ~(SymbolFlags::project | SymbolFlags::irTypeMade | SymbolFlags::owned)));
}

void Symbol::Read(Reader& reader)
{
    sid = reader.GetBinaryReader().ReadUInt();
    flags = SymbolFlags(reader.GetBinaryReader().ReadUShort());
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
    return Source() == SymbolSource::project && (Access() == SymbolAccess::public_ || Serialize());
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

InterfaceTypeSymbol* Symbol::Interface() const
{
    if (IsInterfaceTypeSymbol())
    {
        return const_cast<InterfaceTypeSymbol*>(static_cast<const InterfaceTypeSymbol*>(this));
    }
    else 
    {
        if (parent)
        {
            return parent->Interface();
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

ContainerSymbol* Symbol::ClassInterfaceOrNs() const
{
    if (IsClassTypeSymbol() || IsInterfaceTypeSymbol() || IsNamespaceSymbol())
    {
        return const_cast<ContainerSymbol*>(static_cast<const ContainerSymbol*>(this));
    }
    else
    {
        if (parent)
        {
            return parent->ClassInterfaceOrNs();
        }
        else
        {
            return nullptr;
        }
    }
}

void Symbol::Dump(CodeFormatter& formatter)
{
    std::string f = SymbolFlagStr(flags, DeclaredAccess(), !IsNamespaceSymbol());
    if (!f.empty())
    {
        f.append(1, ' ');
    }
    f.append(TypeString());
    if (!f.empty())
    {
        f.append(1, ' ');
    }
    f.append(Name());
    formatter.WriteLine(f);
}

void Symbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
}

void Symbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
}

void Symbol::InitVirtualFunctionTablesAndInterfaceTables()
{
}

void Symbol::MakeIrType()
{
    SetIrTypeMade();
}

std::string Symbol::FullDocId() const
{
    std::string parentDocId;
    Symbol* p = Parent();
    if (p)
    {
        parentDocId = p->FullDocId();
    }
    if (parentDocId.empty())
    {
        return DocId();
    }
    else
    {
        return parentDocId + "." + DocId();
    }
}

void Symbol::Collect(SymbolCollector& collector)
{
    collector.Add(this);
}

void Symbol::DoSerialize() 
{ 
    SetFlag(SymbolFlags::serialize); 
    if (parent)
    {
        if (!parent->Serialize())
        {
            parent->DoSerialize();
        }
    }
    TypeSymbol* type = GetType();
    if (type)
    {
        type->DoSerialize();
    }
}

TypeSymbol* Symbol::GetType() const
{
    return nullptr;
}

std::string Symbol::Syntax() const
{
    return Name();
}

void Symbol::ReplaceReplicaTypes()
{
}

} } // namespace Cm::Sym
