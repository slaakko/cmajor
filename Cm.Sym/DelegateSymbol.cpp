/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

DelegateTypeSymbol::DelegateTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), flags(DelegateTypeSymbolFlags::none), returnType(nullptr)
{
}

std::string DelegateTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

bool DelegateTypeSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    if (Parent()->IsClassDelegateTypeSymbol()) return true;
    return TypeSymbol::IsExportSymbol();
}

void DelegateTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(uint8_t(flags));
    writer.Write(returnType->Id());
}

void DelegateTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    flags = DelegateTypeSymbolFlags(reader.GetBinaryReader().ReadByte());
    reader.FetchTypeFor(this, 0);
    reader.EnqueueMakeIrTypeFor(this);
}

void DelegateTypeSymbol::AddSymbol(Symbol* symbol)
{
    TypeSymbol::AddSymbol(symbol);
    if (symbol->IsParameterSymbol())
    {
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameters.push_back(parameterSymbol);
    }
}

void DelegateTypeSymbol::SetType(TypeSymbol* type_, int index)
{
    SetReturnType(type_);
}

void DelegateTypeSymbol::SetReturnType(TypeSymbol* returnType_)
{
    returnType = returnType_;
}

void DelegateTypeSymbol::MakeIrType() 
{
    if (IrTypeMade()) return;
    returnType->MakeIrType();
    Ir::Intf::Type* irReturnType = returnType->GetIrType()->Clone();
    std::vector<Ir::Intf::Type*> irParameterTypes;
    for (Cm::Sym::ParameterSymbol* parameter : parameters)
    {
        Cm::Sym::TypeSymbol* parameterType = parameter->GetType();
        parameterType->MakeIrType();
        Ir::Intf::Type* irParameterType = parameterType->GetIrType()->Clone();
        irParameterTypes.push_back(irParameterType);
    }
    if (!IsNothrow())
    {
        Ir::Intf::Type* exceptionCodeParamType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI32(), 1);
        irParameterTypes.push_back(exceptionCodeParamType);
    }
    Ir::Intf::Type* irFunctionType = Cm::IrIntf::CreateFunctionType(irReturnType, irParameterTypes);
    Ir::Intf::Type* irDelegateType = Cm::IrIntf::Pointer(irFunctionType, 1);
    SetIrType(irDelegateType);
    SetDefaultIrValue(Cm::IrIntf::Null(irDelegateType));
}

void DelegateTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (returnType)
    {
        if (returnType->IsDerivedTypeSymbol())
        {
            if (collected.find(returnType) == collected.end())
            {
                collected.insert(returnType);
                returnType->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
            }
        }
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    }
}

void DelegateTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    if (returnType)
    {
        if (collected.find(returnType) == collected.end())
        {
            collected.insert(returnType);
            returnType->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
    }
}

ClassDelegateTypeSymbol::ClassDelegateTypeSymbol(const Span& span_, const std::string& name_) : ClassTypeSymbol(span_, name_), flags(ClassDelegateTypeSymbolFlags::none), returnType(nullptr)
{
}

std::string ClassDelegateTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());

}

bool ClassDelegateTypeSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return ClassTypeSymbol::IsExportSymbol();
}

void ClassDelegateTypeSymbol::SetReturnType(TypeSymbol* returnType_)
{
    returnType = returnType_;
}

void ClassDelegateTypeSymbol::AddSymbol(Symbol* symbol)
{
    ClassTypeSymbol::AddSymbol(symbol);
    if (symbol->IsParameterSymbol())
    {
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameters.push_back(parameterSymbol);
    }
}

void ClassDelegateTypeSymbol::SetType(TypeSymbol* type_, int index)
{
    SetReturnType(type_);
}

void ClassDelegateTypeSymbol::Write(Writer& writer)
{
    ClassTypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(uint8_t(flags));
    writer.Write(returnType->Id());
}

void ClassDelegateTypeSymbol::Read(Reader& reader)
{
    ClassTypeSymbol::Read(reader);
    flags = ClassDelegateTypeSymbolFlags(reader.GetBinaryReader().ReadByte());
    reader.FetchTypeFor(this, 0);
    reader.EnqueueMakeIrTypeFor(this);
}

void ClassDelegateTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (returnType)
    {
        if (returnType->IsDerivedTypeSymbol())
        {
            if (collected.find(returnType) == collected.end())
            {
                collected.insert(returnType);
                returnType->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
            }
        }
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    }
}

void ClassDelegateTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    if (returnType)
    {
        if (collected.find(returnType) == collected.end())
        {
            collected.insert(returnType);
            returnType->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
    }
}

} } // namespace Cm::Sym