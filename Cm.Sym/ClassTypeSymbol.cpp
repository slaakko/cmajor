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
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

PersistentClassData::PersistentClassData() : classNodePos(0), classNodeSize(0)
{
}

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_, TypeId()), flags(ClassTypeSymbolFlags::none), baseClass(nullptr), 
    vptrIndex(-1), destructor(nullptr), staticConstructor(nullptr), initializedVar(nullptr), cid(noCid), key(0), level(0), priority(0), compileUnit(nullptr)
{
}

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::string& name_, bool getNextId_, uint64_t cid_) :
    TypeSymbol(span_, name_, getNextId_ ? TypeId(cid) : TypeId()), flags(ClassTypeSymbolFlags::none), baseClass(nullptr), vptrIndex(-1), destructor(nullptr),
    staticConstructor(nullptr), initializedVar(nullptr), cid(cid_), key(0), level(0), priority(0), compileUnit(nullptr)
{
}

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::string& name_, const TypeId& id_) : TypeSymbol(span_, name_, id_), flags(ClassTypeSymbolFlags::none), baseClass(nullptr), 
    vptrIndex(-1), destructor(nullptr), staticConstructor(nullptr), initializedVar(nullptr), cid(noCid), key(0), level(0), priority(0), compileUnit(nullptr)
{
}

std::string ClassTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

bool ClassTypeSymbol::IsExportSymbol() const
{
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    return TypeSymbol::IsExportSymbol();
}

void ClassTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(cid);
    writer.GetBinaryWriter().Write(uint32_t(flags & ~ClassTypeSymbolFlags::vtblInitialized));
    bool hasBaseClass = baseClass != nullptr;
    writer.GetBinaryWriter().Write(hasBaseClass);
    if (hasBaseClass)
    {
        writer.Write(baseClass->Id());
    }
    bool hasInitializedVar = initializedVar != nullptr;
    writer.GetBinaryWriter().Write(hasInitializedVar);
    if (initializedVar)
    {
        writer.Write(initializedVar.get());
    }
    if (IsClassTemplateSymbol())
    {
        if (!persistentClassData)
        {
            throw std::runtime_error("no persistent class data");
        }
        persistentClassData->usingNodes.Write(writer.GetAstWriter());
        Cm::Ast::Node* node = writer.GetSymbolTable()->GetNode(this);
        if (node->IsClassNode())
        {
            uint64_t sizePos = writer.GetBinaryWriter().Pos();
            writer.GetBinaryWriter().Write(uint64_t(0));
            uint64_t classNodePos = writer.GetBinaryWriter().Pos();
            writer.GetAstWriter().Write(node);
            uint64_t classNodeEndPos = writer.GetBinaryWriter().Pos();
            uint64_t classNodeSize = classNodeEndPos - classNodePos;
            writer.GetBinaryWriter().Seek(sizePos);
            writer.GetBinaryWriter().Write(classNodeSize);
            writer.GetBinaryWriter().Seek(classNodeEndPos);
        }
        else
        {
            throw std::runtime_error("not class node");
        }
    }
    if (!IsTemplateTypeSymbol() && !IsClassDelegateTypeSymbol() && sourceFilePath.empty())
    {
        throw std::runtime_error("source file path not set");
    }
    writer.GetBinaryWriter().Write(sourceFilePath);
}

void ClassTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    cid = reader.GetBinaryReader().ReadULong();
    flags = ClassTypeSymbolFlags(reader.GetBinaryReader().ReadUInt());
    bool hasBaseClass = reader.GetBinaryReader().ReadBool();
    if (hasBaseClass)
    {
        reader.FetchTypeFor(this, -2);
    }
    bool hasInitializedVar = reader.GetBinaryReader().ReadBool();
    if (hasInitializedVar)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsMemberVariableSymbol())
        {
            MemberVariableSymbol* memberVarSymbol = static_cast<MemberVariableSymbol*>(symbol);
            initializedVar.reset(memberVarSymbol);
            initializedVar->SetParent(this);
        }
    }
    if (IsClassTemplateSymbol())
    {
        if (!persistentClassData)
        {
            persistentClassData.reset(new PersistentClassData());
        }
        persistentClassData->usingNodes.Read(reader.GetAstReader());
        persistentClassData->classNodeSize = reader.GetBinaryReader().ReadULong();
        persistentClassData->classNodePos = reader.GetBinaryReader().GetPos();
        persistentClassData->cmlFilePath = reader.GetBinaryReader().FileName();
        reader.GetBinaryReader().Skip(persistentClassData->classNodeSize);
    }
    reader.EnqueueMakeIrTypeFor(this);
    sourceFilePath = reader.GetBinaryReader().ReadString();
}

void ClassTypeSymbol::ReadClassNode(Cm::Sym::SymbolTable& symbolTable, int fileIndex)
{
    if (!persistentClassData)
    {
        throw std::runtime_error("no persistent class data");
    }
    const std::string& cmlFilePath = persistentClassData->cmlFilePath;
    Cm::Ser::BinaryReader binaryReader(cmlFilePath);
    binaryReader.SetPos(persistentClassData->classNodePos);
    Cm::Ast::Reader astReader(binaryReader);
    astReader.SetReplaceFileIndex(fileIndex);
    Cm::Ast::Node* node = astReader.ReadNode();
    if (node->IsClassNode())
    {
        symbolTable.SetNode(this, node);
        persistentClassData->classNode.reset(static_cast<Cm::Ast::ClassNode*>(node));
    }
    else
    {
        throw std::runtime_error("not class node");
    }
}

void ClassTypeSymbol::FreeClassNode(Cm::Sym::SymbolTable& symbolTable)
{
    if (!persistentClassData)
    {
        throw std::runtime_error("no persistent class data");
    }
    if (persistentClassData->classNode)
    {
        persistentClassData->classNode.reset();
        symbolTable.SetNode(this, nullptr);
    }
}

void ClassTypeSymbol::SetType(TypeSymbol* type, int index) 
{
    if (index == -2)
    {
        baseClass = static_cast<ClassTypeSymbol*>(type);
        GetContainerScope()->SetBase(baseClass->GetContainerScope());
    }
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
    if (IsReplica())
    {
        SetFlag(ClassTypeSymbolFlags::vtblInitialized);
        return;
    }
    if (!Bound() && IsTemplateTypeSymbol())
    {
        return;
    }
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
        for (FunctionSymbol* virtualFunction : vtbl)
        {
            if (virtualFunction && virtualFunction->IsAbstract())
            {
                if (!IsAbstract())
                {
                    throw Cm::Sym::Exception("class containing abstract member functions must be declared abstract", GetSpan(), virtualFunction->GetSpan());
                }
            }
        }
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
        vtblToInit.push_back(nullptr); // first entry is reserved for RTTI
    }
    std::vector<FunctionSymbol*> virtualFunctions;
    if (destructor)
    { 
        if (destructor->IsVirtual() || destructor->IsOverride())
        {
            virtualFunctions.push_back(destructor);
        }
    }
    for (Symbol* symbol : Symbols())
    {
        if (symbol->IsFunctionSymbol())
        {
            Cm::Sym::FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
            if (functionSymbol->IsDestructor()) continue;
            if (functionSymbol->IsVirtualAbstractOrOverride())
            {
                virtualFunctions.push_back(functionSymbol);
            }
        }
    }
    int16_t n = int16_t(virtualFunctions.size()) + 1; // number of virtual functions + RTTI entry
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
                if (!f->CanThrow() && v->CanThrow() || f->CanThrow() && !v->CanThrow())
                {
                    throw Exception("overriding function has conflicting nothrow specification compared to base class virtual function", f->GetSpan(), v->GetSpan());
                }
                f->SetVtblIndex(j);
                vtblToInit[j] = f;
                found = true;
                f->SetOverriddenFunction(v);
                v->AddToOverrideSet(f);
                break;
            }
        }
        if (!found)
        {
            if (f->IsOverride())
            {
                throw Exception("no suitable function to override ('" + f->FullName() + "')", f->GetSpan());
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
        else if (functionSymbol->IsConversionFunction())
        {
            AddConversion(functionSymbol);
        }
    }
    else if (symbol->IsTypeParameterSymbol())
    {
        typeParameters.push_back(static_cast<TypeParameterSymbol*>(symbol));
    }
}

void ClassTypeSymbol::AddConversion(FunctionSymbol* functionSymbol)
{
    conversions.insert(functionSymbol);
}

void ClassTypeSymbol::MakeIrType()
{
    if (IrTypeMade()) return;
    SetIrType(Cm::IrIntf::CreateClassTypeName(FullName()));
}

void ClassTypeSymbol::SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_)
{
    if (!persistentClassData)
    {
        persistentClassData.reset(new PersistentClassData());
        for (Cm::Ast::Node* usingNode : usingNodes_)
        {
            Cm::Ast::CloneContext cloneContext;
            persistentClassData->usingNodes.Add(usingNode->Clone(cloneContext));
        }
    }
}

const Cm::Ast::NodeList& ClassTypeSymbol::GetUsingNodes() const
{
    if (!persistentClassData)
    {
        throw std::runtime_error("no persistent class data");
    }
    return persistentClassData->usingNodes;
}

void ClassTypeSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    ContainerSymbol::CollectExportedDerivedTypes(collected, exportedDerivedTypes);
    if (baseClass)
    {
        if (collected.find(baseClass) == collected.end())
        {
            collected.insert(baseClass);
            baseClass->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
        }
    }
}

void ClassTypeSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes)
{
    ContainerSymbol::CollectExportedTemplateTypes(collected, exportedTemplateTypes);
    if (baseClass)
    {
        if (collected.find(baseClass) == collected.end())
        {
            collected.insert(baseClass);
            baseClass->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
}

void ClassTypeSymbol::Dump(CodeFormatter& formatter)
{
    if (!IsProject()) return;
    std::string f = SymbolFlagStr(Flags(), DeclaredAccess(), true);
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
    formatter.Write(f);
    if (!typeParameters.empty())
    {
        formatter.Write("<");
        bool first = true;
        for (TypeParameterSymbol* typeParam : typeParameters)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                formatter.Write(", ");
            }
            formatter.Write(typeParam->Name());
        }
        formatter.Write(">");
    }
    formatter.WriteLine();
    formatter.IncIndent();
    for (Symbol* symbol : Symbols())
    {
        symbol->Dump(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("end of " + TypeString() + " " + Name());
}

std::string ClassTypeSymbol::Syntax() const
{
    std::string syntax = SymbolFlagStr(Flags(), DeclaredAccess(), true);
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    if (IsAbstract())
    {
        syntax.append("abstract ");
    }
    syntax.append("class");
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append(Name()).append(";");
    return syntax;
}

std::string ClassTypeSymbol::FullClassTemplateId() const
{
    std::string fullClassTemplateId = FullDocId();
    for (TypeParameterSymbol* typeParam : typeParameters)
    {
        fullClassTemplateId.append(".").append(typeParam->Name());
    }
    return fullClassTemplateId;
}

void ClassTypeSymbol::ReplaceReplicaTypes()
{
    if (IsClassTemplateSymbol()) return;
    TypeSymbol::ReplaceReplicaTypes();
    if (baseClass && baseClass->IsReplica() && baseClass->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(baseClass);
        baseClass = replica->GetPrimaryTemplateTypeSymbol();
    }
}

} } // namespace Cm::Sym
