/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_, TypeId()), flags(ClassTypeSymbolFlags::none), baseClass(nullptr), vptrIndex(-1), 
    destructor(nullptr), staticConstructor(nullptr), initializedVar(nullptr)
{
}

std::string ClassTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

void ClassTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(uint32_t(flags & ~ClassTypeSymbolFlags::vtblInitialized));
    bool hasBaseClass = baseClass != nullptr;
    writer.GetBinaryWriter().Write(hasBaseClass);
    if (hasBaseClass)
    {
        writer.Write(baseClass->Id());
    }
}

void ClassTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    flags = ClassTypeSymbolFlags(reader.GetBinaryReader().ReadUInt());
    bool hasBaseClass = reader.GetBinaryReader().ReadBool();
    if (hasBaseClass)
    {
        reader.FetchTypeFor(this, 0);
    }
}

void ClassTypeSymbol::SetType(TypeSymbol* type, int index) 
{
    baseClass = static_cast<ClassTypeSymbol*>(type);
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

bool ClassTypeSymbol::DoGenerateDestructor()
{
    if (destructor) return false;           // already has one, don't generate
    if (GenerateDestructor()) return true;  // asked to generate one, so generate
    if (IsVirtual()) return true;           // virtual classes need to have one
    if (HasNonTrivialMemberDestructor()) return true;   // member has one => need to have one
    if (baseClass && baseClass->Destructor()) return true;  // base class has one => need to have one
    return false;                           // else don't generate
}

bool ClassTypeSymbol::DoGenerateStaticConstructor()
{
    if (staticConstructor) return false;    // already has one, don't generate
    if (!staticMemberVariables.empty()) return true;    //  has static data => generate
    return false;
}

void ClassTypeSymbol::SetInitializedVar(MemberVariableSymbol* initializedVar_)
{
    initializedVar.reset(initializedVar_);
}

bool ClassTypeSymbol::HasNonTrivialMemberDestructor() const
{
    for (MemberVariableSymbol* memberVariableSymbol : memberVariables)
    {
        if (!memberVariableSymbol->GetType()->IsClassTypeSymbol()) continue;
        ClassTypeSymbol* memberVarClassType = static_cast<ClassTypeSymbol*>(memberVariableSymbol->GetType());
        if (memberVarClassType->Destructor()) return true;
    }
    return false;
}

ClassTypeSymbol* ClassTypeSymbol::VPtrContainerClass() const
{
    if (vptrIndex != -1) return const_cast<ClassTypeSymbol*>(this);
    if (baseClass) return baseClass->VPtrContainerClass();
    return nullptr;
}

void ClassTypeSymbol::InitVirtualFunctionTables()
{
    TypeSymbol::InitVirtualFunctionTables();
    InitVtbl();
}

void ClassTypeSymbol::InitVtbl()
{
    if (GetFlag(ClassTypeSymbolFlags::vtblInitialized)) return;
    SetFlag(ClassTypeSymbolFlags::vtblInitialized);
    if (baseClass)
    {
        baseClass->InitVtbl();
        if (baseClass->IsVirtual())
        {
            SetVirtual();
        }
    }
    if (IsVirtual())
    {
        if (!baseClass || !baseClass->IsVirtual())
        {
            if (baseClass)
            {
                SetVPtrIndex(1);
            }
            else
            {
                SetVPtrIndex(0);
            }
        }
        InitVtbl(vtbl);
    }
}

bool Overrides(Cm::Sym::FunctionSymbol* f, Cm::Sym::FunctionSymbol* g)
{
    if (!f->IsMemberFunctionSymbol() || !g->IsMemberFunctionSymbol()) return false;
    if (f->GroupName() == g->GroupName())
    {
        int n = int(f->Parameters().size());
        if (n == int(g->Parameters().size()))
        {
            for (int i = 1; i < n; ++i)
            {
                ParameterSymbol* p = f->Parameters()[i];
                ParameterSymbol* q = g->Parameters()[i];
                if (!TypesEqual(p->GetType(), q->GetType())) return false;
            }
            return true;
        }
    }
    return false;
}

void ClassTypeSymbol::InitVtbl(std::vector<Cm::Sym::FunctionSymbol*>& vtblToInit)
{
    if (!IsVirtual()) return;
    if (baseClass)
    {
        baseClass->InitVtbl(vtblToInit);
    }
    if (vtblToInit.empty())
    {
        vtblToInit.push_back(nullptr); // first entry is reserved for class name 
    }
    std::vector<FunctionSymbol*> virtualFunctions;
    if (destructor)
    { 
        if (destructor->IsVirtual() || destructor->IsOverride())
        {
            virtualFunctions.push_back(destructor);
        }
    }
    for (const std::unique_ptr<Symbol>& symbol : Symbols())
    {
        if (symbol->IsFunctionSymbol())
        {
            Cm::Sym::FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol.get());
            if (functionSymbol->IsDestructor()) continue;
            if (functionSymbol->IsVirtualAbstractOrOverride())
            {
                virtualFunctions.push_back(functionSymbol);
            }
        }
    }
    int16_t n = int16_t(virtualFunctions.size()) + 1; // number of virtual functions + class name entry
    for (int16_t i = 1; i < n; ++i)
    {
        FunctionSymbol* f = virtualFunctions[i - 1];
        bool found = false;
        int16_t m = int16_t(vtblToInit.size());
        for (int16_t j = 1; j < m; ++j)
        {
            FunctionSymbol* v = vtblToInit[j];
            if (Overrides(f, v))
            {
                if (!f->IsOverride())
                {
                    throw Exception("overriding function should be declared with override specifier", f->GetSpan()); 
                }
                if (f->IsNothrow() && !v->IsNothrow() || !f->IsNothrow() && v->IsNothrow())
                {
                    throw Exception("overriding function has conflicting nothrow specification compared to base class virtual function", f->GetSpan(), v->GetSpan());
                }
                f->SetVtblIndex(j);
                vtblToInit[j] = f;
                found = true;
                break;
            }
        }
        if (!found)
        {
            if (f->IsOverride())
            {
                throw Exception("no suitable function to override", f->GetSpan());
            }
            f->SetVtblIndex(m);
            vtblToInit.push_back(f);
        }
    }
}

void ClassTypeSymbol::AddSymbol(Symbol* symbol)
{
    TypeSymbol::AddSymbol(symbol);
    if (symbol->IsMemberVariableSymbol())
    {
        MemberVariableSymbol* memberVariableSymbol = static_cast<MemberVariableSymbol*>(symbol);
        if (memberVariableSymbol->IsStatic())
        {
            staticMemberVariables.push_back(memberVariableSymbol);
        }
        else
        {
            memberVariables.push_back(memberVariableSymbol);
        }
    }
    else if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        if (functionSymbol->IsDestructor())
        {
            destructor = functionSymbol;
        }
        else if (functionSymbol->IsStaticConstructor())
        {
            staticConstructor = functionSymbol;
        }
        else if (functionSymbol->IsConvertingConstructor())
        { 
            AddConversion(functionSymbol);
        }
    }
}

void ClassTypeSymbol::AddConversion(FunctionSymbol* functionSymbol)
{
    conversions.insert(functionSymbol);
}

void ClassTypeSymbol::MakeIrType()
{
    SetIrType(Cm::IrIntf::CreateClassTypeName(FullName()));
}

} } // namespace Cm::Sym
