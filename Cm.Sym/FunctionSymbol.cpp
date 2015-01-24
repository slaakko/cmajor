/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

FunctionLookup::FunctionLookup(ScopeLookup lookup_, ContainerScope* scope_) : lookup(lookup_), scope(scope_)
{
}

FunctionSymbol::FunctionSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_), returnType(nullptr), compileUnit(nullptr), flags(FunctionSymbolFlags::none), vtblIndex(-1)
{
}

FunctionLookupSet::FunctionLookupSet()
{
}

void FunctionLookupSet::Add(const FunctionLookup& lookup)
{
    FunctionLookup toInsert = lookup;
    for (const FunctionLookup& existingLookup : lookups)
    {
        if (toInsert.Scope() == existingLookup.Scope())
        {
            toInsert.SetLookup(toInsert.Lookup() & ~existingLookup.Lookup());
        }
    }
    if (toInsert.Lookup() != ScopeLookup::none)
    {
        lookups.push_back(toInsert);
    }
}

void FunctionSymbol::AddSymbol(Symbol* symbol)
{
    ContainerSymbol::AddSymbol(symbol);
    if (symbol->IsParameterSymbol())
    {
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameters.push_back(parameterSymbol);
    }
}

void FunctionSymbol::SetReturnType(TypeSymbol* returnType_)
{
    returnType = returnType_;
}

bool FunctionSymbol::IsConstructor() const
{
    return groupName == "@constructor" && !IsStatic();
}

bool FunctionSymbol::IsDefaultConstructor() const
{
    return groupName == "@constructor" && !IsStatic() && parameters.size() == 1;
}

bool FunctionSymbol::IsCopyConstructor() const
{
    if (groupName == "@constructor" && !IsStatic() && parameters.size() == 2)
    {
        Cm::Sym::TypeSymbol* firstParamType = parameters[0]->GetType();
        Cm::Sym::TypeSymbol* classType = firstParamType->GetBaseType();
        Cm::Sym::TypeSymbol* secondParamType = parameters[1]->GetType();
        if (secondParamType->IsConstReferenceType() && TypesEqual(secondParamType->GetBaseType(), classType))
        {
            return true;
        }
    }
    return false;
}

bool FunctionSymbol::IsMoveConstructor() const
{
    if (groupName == "@constructor" && !IsStatic() && parameters.size() == 2)
    {
        Cm::Sym::TypeSymbol* firstParamType = parameters[0]->GetType();
        Cm::Sym::TypeSymbol* classType = firstParamType->GetBaseType();
        Cm::Sym::TypeSymbol* secondParamType = parameters[1]->GetType();
        if (secondParamType->IsRvalueRefType() && TypesEqual(secondParamType->GetBaseType(), classType))
        {
            return true;
        }
    }
    return false;
}

bool FunctionSymbol::IsStaticConstructor() const
{
    if (groupName == "@static_constructor" && IsStatic() && parameters.empty())
    {
        return true;
    }
    return false;
}

bool FunctionSymbol::IsCopyAssignment() const
{
    if (groupName == "operator=" && parameters.size() == 2)
    {
        Cm::Sym::TypeSymbol* firstParamType = parameters[0]->GetType();
        Cm::Sym::TypeSymbol* classType = firstParamType->GetBaseType();
        Cm::Sym::TypeSymbol* secondParamType = parameters[1]->GetType();
        if (secondParamType->IsConstReferenceType() && TypesEqual(secondParamType->GetBaseType(), classType))
        {
            return true;
        }
    }
    return false;
}

bool FunctionSymbol::IsMoveAssignment() const
{
    if (groupName == "operator=" && parameters.size() == 2)
    {
        Cm::Sym::TypeSymbol* firstParamType = parameters[0]->GetType();
        Cm::Sym::TypeSymbol* classType = firstParamType->GetBaseType();
        Cm::Sym::TypeSymbol* secondParamType = parameters[1]->GetType();
        if (secondParamType->IsRvalueRefType() && TypesEqual(secondParamType->GetBaseType(), classType))
        {
            return true;
        }
    }
    return false;
}

bool FunctionSymbol::IsDestructor() const
{
    if (groupName == "@destructor" && parameters.size() == 1)
    {
        return true;
    }
    return false;
}

void FunctionSymbol::Write(Writer& writer)
{
    ContainerSymbol::Write(writer);
    writer.GetBinaryWriter().Write(uint16_t(flags));
    writer.GetBinaryWriter().Write(groupName);
    writer.GetBinaryWriter().Write(vtblIndex);
    bool hasReturnType = returnType != nullptr;
    writer.GetBinaryWriter().Write(hasReturnType);
    if (hasReturnType)
    {
        writer.Write(returnType->Id());
    }
}

void FunctionSymbol::Read(Reader& reader)
{
    ContainerSymbol::Read(reader);
    flags = FunctionSymbolFlags(reader.GetBinaryReader().ReadUShort());
    groupName = reader.GetBinaryReader().ReadString();
    vtblIndex = reader.GetBinaryReader().ReadShort();
    bool hasReturnType = reader.GetBinaryReader().ReadBool();
    if (hasReturnType)
    {
        reader.FetchTypeFor(this, 0);
    }
}

void FunctionSymbol::SetType(TypeSymbol* type_, int index)
{
    SetReturnType(type_);
}

void FunctionSymbol::ComputeName()
{
    std::string s;
    if (returnType)
    {
        s.append(returnType->FullName()).append(1, ' ');
    }
    s.append(groupName);
    s.append(1, '(');
    bool first = true;
    for (ParameterSymbol* parameter : parameters)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        TypeSymbol* paramType = parameter->GetType();
        s.append(paramType->FullName());
    }
    s.append(1, ')');
    SetName(s);
}

TypeSymbol* FunctionSymbol::GetTargetType() const
{
    throw std::runtime_error("member function not applicable");
}

void FunctionSymbol::CollectExportedDerivedTypes(std::vector<TypeSymbol*>& exportedDerivedTypes) 
{
    if (returnType)
    {
        if (returnType->IsDerivedTypeSymbol())
        {
            returnType->CollectExportedDerivedTypes(exportedDerivedTypes);
        }
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->CollectExportedDerivedTypes(exportedDerivedTypes);
    }
}

} } // namespace Cm::Sym