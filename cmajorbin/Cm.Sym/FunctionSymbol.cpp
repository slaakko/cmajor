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
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Clone.hpp>
#include <stdexcept>

namespace Cm { namespace Sym {

std::string FunctionSymbolFlagString(FunctionSymbolFlags flags)
{
    std::string s;
    if ((flags & FunctionSymbolFlags::cdecl_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("cdecl");
    }
    if ((flags & FunctionSymbolFlags::virtual_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("virtual");
    }
    if ((flags & FunctionSymbolFlags::abstract_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("abstract");
    }
    if ((flags & FunctionSymbolFlags::override_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("override");
    }
    if ((flags & FunctionSymbolFlags::nothrow) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("nothrow");
    }
    if ((flags & FunctionSymbolFlags::inline_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("inline");
    }
    if ((flags & FunctionSymbolFlags::suppressed) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("suppressed");
    }
    if ((flags & FunctionSymbolFlags::default_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("default");
    }
    if ((flags & FunctionSymbolFlags::explicit_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("explicit");
    }
    if ((flags & FunctionSymbolFlags::new_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("new");
    }
    return s;
}

FunctionLookup::FunctionLookup(ScopeLookup lookup_, ContainerScope* scope_) : lookup(lookup_), scope(scope_)
{
}

PersistentFunctionData::PersistentFunctionData(): bodyPos(0), bodySize(0), specifiers(), returnTypeExprNode(nullptr), groupId(nullptr), constraint(nullptr), functionPos(0), functionSize(0)
{
}

FunctionSymbol::FunctionSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_), returnType(nullptr), compileUnit(nullptr), flags(FunctionSymbolFlags::none), vtblIndex(-1),
    classObjectResultIrParam(nullptr), mutexId(-1), overriddenFunction(nullptr), functionTemplate(nullptr), returnValueSymbol(nullptr)
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
    else if (symbol->IsTypeParameterSymbol())
    {
        TypeParameterSymbol* typeParameterSymbol = static_cast<TypeParameterSymbol*>(symbol);
        typeParameterSymbol->SetIndex(int(typeParameters.size()));
        typeParameters.push_back(typeParameterSymbol);
    }
    else if (symbol->IsReturnValueSymbol())
    {
        returnValueSymbol = static_cast<ReturnValueSymbol*>(symbol);
    }
}

void FunctionSymbol::SetReturnType(TypeSymbol* returnType_)
{
    returnType = returnType_;
}

bool FunctionSymbol::ReturnsClassObjectByValue() const
{
    return returnType && returnType->IsClassTypeSymbol();
}

ParameterSymbol* FunctionSymbol::ThisParameter() const
{
    return parameters[0];
}

ReturnValueSymbol* FunctionSymbol::ReturnValue() const
{
    return returnValueSymbol;
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

bool FunctionSymbol::IsConvertingConstructor() const
{
    return IsConstructor() && GetFlag(FunctionSymbolFlags::conversion);
}

bool FunctionSymbol::IsConversionFunction() const
{
    return !IsConstructor() && GetFlag(FunctionSymbolFlags::conversion);
}

bool FunctionSymbol::IsExportSymbol() const 
{
    if (IsFunctionTemplateSpecialization()) return false;
    if (Parent()->IsClassTemplateSymbol()) return false;
    if (Parent()->IsTemplateTypeSymbol()) return false;
    if (IsReplica()) return false;
    if (Source() == SymbolSource::project && GetGlobalFlag(GlobalFlags::optimize) && IsInline()) return true;
    return ContainerSymbol::IsExportSymbol();
}

void FunctionSymbol::SetConvertingConstructor()
{
    SetFlag(FunctionSymbolFlags::conversion);
}

void FunctionSymbol::SetConversionFunction()
{
    SetFlag(FunctionSymbolFlags::conversion);
}

bool FunctionSymbol::CheckIfConvertingConstructor() const
{
    return IsConstructor() && parameters.size() == 2 && !IsExplicit() && !IsStatic() && !IsCopyConstructor() && !IsMoveConstructor();
}

void FunctionSymbol::SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_)
{
    if (!persistentFunctionData)
    {
        persistentFunctionData.reset(new PersistentFunctionData());
        for (Cm::Ast::Node* usingNode : usingNodes_)
        {
            Cm::Ast::CloneContext cloneContext;
            persistentFunctionData->usingNodes.Add(usingNode->Clone(cloneContext));
        }
    }
}

const Cm::Ast::NodeList& FunctionSymbol::GetUsingNodes() const
{
    if (!persistentFunctionData)
    {
        throw std::runtime_error("no persistent function data");
    }
    return persistentFunctionData->usingNodes;
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
    writer.GetBinaryWriter().Write(uint32_t(flags));
    writer.GetBinaryWriter().Write(groupName);
    writer.GetBinaryWriter().Write(vtblIndex);
    if (IsFunctionTemplate())
    {
        SymbolTable* symbolTable = writer.GetSymbolTable();
        Cm::Ast::Node* node = symbolTable->GetNode(this);
        if (!node)
        {
            throw std::runtime_error("write: function node not found from symbol table");
        }
        if (node->IsFunctionNode())
        {
            Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
            bool hasReturnType = functionNode->ReturnTypeExpr();
            writer.GetBinaryWriter().Write(hasReturnType);
            if (hasReturnType)
            {
                writer.GetAstWriter().Write(functionNode->ReturnTypeExpr());
            }
            if (!functionNode->Body())
            {
                throw std::runtime_error("write: function node has no body");
            }
            writer.GetAstWriter().Write(functionNode->GetSpecifiers());
            writer.GetAstWriter().Write(functionNode->GroupId());
            bool hasConstraint = functionNode->Constraint() != nullptr;
            writer.GetBinaryWriter().Write(hasConstraint);
            if (hasConstraint)
            {
                writer.GetAstWriter().Write(functionNode->Constraint());
            }
            if (persistentFunctionData)
            {
                writer.GetBinaryWriter().Write(true);
                persistentFunctionData->usingNodes.Write(writer.GetAstWriter());
            }
            else
            {
                writer.GetBinaryWriter().Write(false);
            }
            uint64_t sizePos = writer.GetBinaryWriter().Pos();
            writer.GetBinaryWriter().Write(uint64_t(0));
            uint64_t bodyPos = writer.GetBinaryWriter().Pos();
            writer.GetAstWriter().Write(functionNode->Body());
            uint64_t bodyEndPos = writer.GetBinaryWriter().Pos();
            uint64_t bodySize = bodyEndPos - bodyPos;
            writer.GetBinaryWriter().Seek(sizePos);
            writer.GetBinaryWriter().Write(bodySize);
            writer.GetBinaryWriter().Seek(bodyEndPos);
        }
        else
        {
            throw std::runtime_error("write: not function node");
        }
    }
    else if (IsInline() && GetGlobalFlag(GlobalFlags::optimize) && !IsMemberOfClassTemplate())
    {
        if (persistentFunctionData)
        {
            writer.GetBinaryWriter().Write(true);
            persistentFunctionData->usingNodes.Write(writer.GetAstWriter());
        }
        else
        {
            writer.GetBinaryWriter().Write(false);
        }
        SymbolTable* symbolTable = writer.GetSymbolTable();
        Cm::Ast::Node* node = symbolTable->GetNode(this);
        if (!node)
        {
            throw std::runtime_error("write: function node not found from symbol table");
        }
        if (node->IsFunctionNode())
        {
            Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
            uint64_t sizePos = writer.GetBinaryWriter().Pos();
            writer.GetBinaryWriter().Write(uint64_t(0));
            uint64_t functionPos = writer.GetBinaryWriter().Pos();
            writer.GetAstWriter().Write(functionNode);
            uint64_t functionEndPos = writer.GetBinaryWriter().Pos();
            uint64_t functionSize = functionEndPos - functionPos;
            writer.GetBinaryWriter().Seek(sizePos);
            writer.GetBinaryWriter().Write(functionSize);
            writer.GetBinaryWriter().Seek(functionEndPos);
            bool hasReturnType = returnType != nullptr;
            writer.GetBinaryWriter().Write(hasReturnType);
            if (hasReturnType)
            {
                writer.Write(returnType->Id());
            }
        }
        else
        {
            throw std::runtime_error("write: not function node");
        }
    }
    else
    {
        bool hasReturnType = returnType != nullptr;
        writer.GetBinaryWriter().Write(hasReturnType);
        if (hasReturnType)
        {
            writer.Write(returnType->Id());
        }
    }
}

void FunctionSymbol::Read(Reader& reader)
{
    ContainerSymbol::Read(reader);
    flags = FunctionSymbolFlags(reader.GetBinaryReader().ReadUInt());
    groupName = reader.GetBinaryReader().ReadString();
    vtblIndex = reader.GetBinaryReader().ReadShort();
    if (IsFunctionTemplate())
    {
        persistentFunctionData.reset(new PersistentFunctionData());
        bool hasReturnType = reader.GetBinaryReader().ReadBool();
        if (hasReturnType)
        {
            persistentFunctionData->returnTypeExprNode.reset(reader.GetAstReader().ReadNode());
        }
        persistentFunctionData->specifiers = reader.GetAstReader().ReadSpecifiers();
        persistentFunctionData->groupId.reset(reader.GetAstReader().ReadFunctionGroupIdNode());
        bool hasConstraint = reader.GetBinaryReader().ReadBool();
        if (hasConstraint)
        {
            persistentFunctionData->constraint.reset(reader.GetAstReader().ReadWhereConstraintNode());
        }
        bool hasUsingNodes = reader.GetBinaryReader().ReadBool();
        if (hasUsingNodes)
        {
            persistentFunctionData->usingNodes.Read(reader.GetAstReader());
        }
        persistentFunctionData->bodySize = reader.GetBinaryReader().ReadULong();
        persistentFunctionData->bodyPos = reader.GetBinaryReader().GetPos();
        persistentFunctionData->cmlFilePath = reader.GetBinaryReader().FileName();
        reader.GetBinaryReader().Skip(persistentFunctionData->bodySize);
    }
    else if (IsInline() && GetGlobalFlag(GlobalFlags::optimize) && !IsMemberOfClassTemplate())
    {
        persistentFunctionData.reset(new PersistentFunctionData());
        bool hasUsingNodes = reader.GetBinaryReader().ReadBool();
        if (hasUsingNodes)
        {
            persistentFunctionData->usingNodes.Read(reader.GetAstReader());
        }
        persistentFunctionData->functionSize = reader.GetBinaryReader().ReadULong();
        persistentFunctionData->functionPos = reader.GetBinaryReader().GetPos();
        persistentFunctionData->cmlFilePath = reader.GetBinaryReader().FileName();
        reader.GetBinaryReader().Skip(persistentFunctionData->functionSize);
        bool hasReturnType = reader.GetBinaryReader().ReadBool();
        if (hasReturnType)
        {
            reader.FetchTypeFor(this, 0);
        }
    }
    else
    {
        bool hasReturnType = reader.GetBinaryReader().ReadBool();
        if (hasReturnType)
        {
            reader.FetchTypeFor(this, 0);
        }
    }
}

void FunctionSymbol::ReadFunctionNode(Cm::Sym::SymbolTable& symbolTable, int fileIndex)
{
    if (!persistentFunctionData)
    {
        throw std::runtime_error("no persistent function data");
    }
    const std::string& cmlFilePath = persistentFunctionData->cmlFilePath;
    Cm::Ser::BinaryReader binaryReader(cmlFilePath);
    binaryReader.SetPos(persistentFunctionData->functionPos);
    Cm::Ast::Reader astReader(binaryReader);
    astReader.SetReplaceFileIndex(fileIndex);
    Cm::Ast::Node* node = astReader.ReadNode();
    if (node->IsFunctionNode())
    {
        symbolTable.SetNode(this, node);
        persistentFunctionData->functionNode.reset(static_cast<Cm::Ast::FunctionNode*>(node));
    }
    else
    {
        throw std::runtime_error("not function node");
    }
}

void FunctionSymbol::FreeFunctionNode(Cm::Sym::SymbolTable& symbolTable)
{
    if (!persistentFunctionData)
    {
        throw std::runtime_error("no persistent function data");
    }
    if (persistentFunctionData->functionNode)
    {
        persistentFunctionData->functionNode.reset();
        symbolTable.SetNode(this, nullptr);
    }
}

void FunctionSymbol::SetType(TypeSymbol* type_, int index)
{
    SetReturnType(type_);
}

void FunctionSymbol::ComputeName()
{
    std::string s;
    if (IsConversionFunction())
    {
        groupName = "operator_" + GetReturnType()->FullName();
    }
    s.append(groupName);
    if (!typeArguments.empty())
    {
        s.append(1, '<');
        bool first = true;
        for (Cm::Sym::TypeSymbol* typeArgument : typeArguments)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                s.append(", ");
            }
            s.append(typeArgument->FullName());
        }
        s.append(1, '>');
    }
    bool isConst = false;
    s.append(1, '(');
    bool first = true;
    for (ParameterSymbol* parameter : parameters)
    {
        if (first)
        {
            first = false;
            if (IsMemberFunctionSymbol())
            {
                if (parameter->GetType()->IsConstType())
                {
                    isConst = true;
                }
            }
        }
        else
        {
            s.append(", ");
        }
        TypeSymbol* paramType = parameter->GetType();
        s.append(paramType->FullName());
    }
    s.append(1, ')');
    if (isConst)
    {
        s.append(" const");
    }
    SetName(s);
}

bool FunctionSymbol::IsConst() const
{
    if (IsMemberFunctionSymbol() && !parameters.empty())
    {
        if (parameters[0]->GetType()->IsConstType())
        {
            return true;
        }
    }
    return false;
}

std::string FunctionSymbol::FullDocId() const
{
    std::string fullDocId;
    Symbol* p = Parent();
    if (p)
    {
        fullDocId = p->FullDocId();
    }
    if (!fullDocId.empty())
    {
        fullDocId.append(1, '.');
    }
    fullDocId.append(Cm::Sym::MakeGroupDocId(groupName));
    if (!typeParameters.empty())
    {
        for (TypeParameterSymbol* typeParam : typeParameters)
        {
            fullDocId.append(1, '.').append(typeParam->Name());
        }
    }
    if (!typeArguments.empty())
    {
        for (TypeSymbol* typeArgument : typeArguments)
        {
            fullDocId.append(1, '.').append(typeArgument->Name());
        }
    }
    if (!IsFunctionTemplate())
    {
        for (ParameterSymbol* parameter : parameters)
        {
            fullDocId.append(1, '.').append(parameter->GetType()->FullDocId());
        }
    }
    if (!constraintDocId.empty())
    {
        fullDocId.append(1, '.').append(constraintDocId);
    }
    return fullDocId;
}

std::string FunctionSymbol::Syntax() const
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
    if (returnType)
    {
        syntax.append(returnType->FullName()).append(" ");
    }
    int startParamIndex = 0;
    if (IsMemberFunctionSymbol())
    {
        if (!IsStatic())
        {
            startParamIndex = 1;
        }
        Symbol* parent = Parent();
        ClassTypeSymbol* classType = nullptr;
        if (parent->IsClassTypeSymbol())
        {
            classType = static_cast<ClassTypeSymbol*>(parent);
        }
        else
        {
            throw std::runtime_error("not class type");
        }
        if (IsConstructor())
        {
            syntax.append(classType->Name());
        }
        else if (IsDestructor())
        {
            syntax.append("~").append(classType->Name());
        }
        else
        {
            syntax.append(groupName);
        }
    }
    else
    {
        syntax.append(groupName);
    }
    syntax.append("(");
    int n = int(Parameters().size());
    for (int i = startParamIndex; i < n; ++i)
    {
        if (i > startParamIndex)
        {
            syntax.append(", ");
        }
        ParameterSymbol* param = Parameters()[i];
        syntax.append(param->GetType()->FullName()).append(" ").append(param->Name());
    }
    syntax.append(")");
    if (IsConst())
    {
        syntax.append(" const");
    }
    syntax.append(";");
    return syntax;
}

std::string FunctionSymbol::ParsingName() const
{
    std::string parsingName;
    int startParamIndex = 0;
    if (IsMemberFunctionSymbol())
    {
        if (!IsStatic())
        {
            startParamIndex = 1;
        }
        Symbol* parent = Parent();
        ClassTypeSymbol* classType = nullptr;
        if (parent->IsClassTypeSymbol())
        {
            classType = static_cast<ClassTypeSymbol*>(parent);
        }
        else
        {
            throw std::runtime_error("not class type");
        }
        if (IsConstructor())
        {
            parsingName.append(classType->Name());
        }
        else if (IsDestructor())
        {
            parsingName.append("~").append(classType->Name());
        }
        else
        {
            parsingName.append(groupName);
        }
    }
    else
    {
        parsingName.append(groupName);
    }
    parsingName.append("(");
    int n = int(Parameters().size());
    for (int i = startParamIndex; i < n; ++i)
    {
        if (i > startParamIndex)
        {
            parsingName.append(", ");
        }
        ParameterSymbol* param = Parameters()[i];
        parsingName.append(param->GetType()->FullName());
    }
    parsingName.append(")");
    if (IsConst())
    {
        parsingName.append(" const");
    }
    return parsingName;
}

TypeSymbol* FunctionSymbol::GetTargetType() const
{
    if (IsConvertingConstructor())
    {
        return parameters[0]->GetType()->GetBaseType();
    }
    else if (IsConversionFunction())
    {
        return returnType;
    }
    else
    {
        throw std::runtime_error("not converting constructor or conversion function");
    }
}

TypeSymbol* FunctionSymbol::GetSourceType() const
{
    if (IsConvertingConstructor())
    {
        return parameters[1]->GetType()->GetBaseType();
    }
    else if (IsConversionFunction())
    {
        return parameters[0]->GetType()->GetBaseType();
    }
    else
    {
        throw std::runtime_error("not converting constructor or conversion function");
    }
}

void FunctionSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
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

void FunctionSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, TemplateTypeSymbol*, TypeIdHash>& exportedTemplateTypes)
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

void FunctionSymbol::Dump(CodeFormatter& formatter)
{
    if (!IsProject()) return;
    if (IsBasicTypeOp()) return;
    std::string s = SymbolFlagStr(Flags(), DeclaredAccess(), true);
    std::string fs = FunctionSymbolFlagString(flags);
    if (!fs.empty())
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append(fs);
    }
    if (returnType)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append(returnType->FullName());
    }
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(groupName);
    if (!typeParameters.empty())
    {
        std::string t = "<";
        bool first = true;
        for (TypeParameterSymbol* typeParam : typeParameters)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                t.append(", ");
            }
            t.append(typeParam->Name());
        }
        t.append(">");
        s.append(t);
    }
    std::string p = "(";
    bool first = true;
    for (ParameterSymbol* param : parameters)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            p.append(", ");
        }
        if (param->GetType())
        {
            p.append(param->GetType()->FullName() + " ");
        }
        p.append(param->Name());
    }
    p.append(")");
    s.append(p);
    formatter.WriteLine(s);
}

FileScope* FunctionSymbol::GetFileScope(ContainerScope* containerScope)
{
    if (!persistentFunctionData->functionFileScope)
    {
        persistentFunctionData->functionFileScope.reset(new FileScope());
        for (const std::unique_ptr<Cm::Ast::Node>& usingNode : persistentFunctionData->usingNodes)
        {
            Cm::Ast::NodeType nodeType = usingNode->GetNodeType();
            switch (nodeType)
            {
                case Cm::Ast::NodeType::aliasNode:
                {
                    Cm::Ast::AliasNode* aliasNode = static_cast<Cm::Ast::AliasNode*>(usingNode.get());
                    persistentFunctionData->functionFileScope->InstallAlias(containerScope, aliasNode);
                    break;
                }
                case Cm::Ast::NodeType::namespaceImportNode:
                {
                    Cm::Ast::NamespaceImportNode* namespaceImportNode = static_cast<Cm::Ast::NamespaceImportNode*>(usingNode.get());
                    persistentFunctionData->functionFileScope->InstallNamespaceImport(containerScope, namespaceImportNode);
                    break;
                }
            }
        }
    }
    return persistentFunctionData->functionFileScope.get();
}

void FunctionSymbol::SetGlobalNs(Cm::Ast::NamespaceNode* globalNs_)
{
    globalNs.reset(globalNs_);
}

void FunctionSymbol::AddToOverrideSet(FunctionSymbol* overrideFun)
{
    overrideSet.insert(overrideFun);
    if (overriddenFunction)
    {
        overriddenFunction->AddToOverrideSet(overrideFun);
    }
}

void FunctionSymbol::SetConstraintDocId(const std::string& constraintDocId_)
{
    constraintDocId = constraintDocId_;
}

void FunctionSymbol::ReplaceReplicaTypes()
{
    if (IsFunctionTemplate()) return;
    ContainerSymbol::ReplaceReplicaTypes();
    if (returnType && returnType->IsReplica() && returnType->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(returnType);
        returnType = replica->GetPrimaryTemplateTypeSymbol();
    }
    for (TypeSymbol*& typeSymbol : typeArguments)
    {
        if (typeSymbol->IsReplica() && typeSymbol->IsTemplateTypeSymbol())
        {
            TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(typeSymbol);
            typeSymbol = replica->GetPrimaryTemplateTypeSymbol();
        }
    }
}

void FunctionSymbol::DoSerialize()
{
    ContainerSymbol::DoSerialize();
    if (returnType)
    {
        returnType->DoSerialize();
    }
    for (ParameterSymbol* parameter : parameters)
    {
        parameter->DoSerialize();
    }
}

} } // namespace Cm::Sym
