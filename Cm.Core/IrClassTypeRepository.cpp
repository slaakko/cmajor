/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Llvm.Ir/Type.hpp>
#include <stdexcept>
#include <algorithm>

namespace Cm { namespace Core {

IrClassTypeRepository::~IrClassTypeRepository()
{
}

struct CidLess
{
    bool operator()(Cm::Sym::ClassTypeSymbol* left, Cm::Sym::ClassTypeSymbol* right) const
    {
        return left->Cid() < right->Cid();
    }
};

void IrClassTypeRepository::SetLayoutIndeces()
{
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        Cm::Sym::ClassTypeSymbol* classType = p.second;
        int index = 0;
        if (classType->BaseClass())
        {
            ++index;
        }
        if (!classType->MemberVariables().empty())
        {
            for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
            {
                if (index == classType->VPtrIndex())
                {
                    ++index;
                }
                memberVariable->SetLayoutIndex(index);
                ++index;
            }
        }
    }
}

void IrClassTypeRepository::Write(Cm::Sym::BcuWriter& writer)
{
    std::vector<Cm::Sym::ClassTypeSymbol*> classTypeVec;
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        Cm::Sym::ClassTypeSymbol* classType = p.second;
        Cm::Sym::ClassTypeSymbol* baseClass = classType->BaseClass();
        while (baseClass)
        {
            if (baseClass->IsTemplateTypeSymbol() && baseClass->IsVirtual())
            {
                classTypeVec.push_back(baseClass);
            }
            baseClass = baseClass->BaseClass();
        }
        classTypeVec.push_back(classType);
    }
    std::sort(classTypeVec.begin(), classTypeVec.end(), CidLess());
    writer.GetSymbolWriter().PushExportMemberVariablesAndFunctionSymbols(true);
    int n = int(classTypeVec.size());
    writer.GetBinaryWriter().Write(int(n));
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ClassTypeSymbol* classType = classTypeVec[i];
        writer.Write(classType);
    }
    writer.GetSymbolWriter().PopExportMemberVariablesAndFunctionSymbols();
}

void IrClassTypeRepository::Read(Cm::Sym::BcuReader& reader)
{
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ClassTypeSymbol* classType = reader.ReadClassTypeSymbol();
        AddClassType(classType);
    }
}

bool IrClassTypeRepository::Added(Cm::Sym::ClassTypeSymbol* classType) const
{
    return classTypeMap.find(classType->FullName()) != classTypeMap.cend();
}

void IrClassTypeRepository::AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    classTypeMap[classTypeSymbol->FullName()] = classTypeSymbol;
}

void LlvmIrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository, 
    const std::vector<Ir::Intf::Type*>& tdfs)
{
    WriteDestructionNodeDef(codeFormatter);
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        Cm::Sym::ClassTypeSymbol* classType = p.second;
        if (!classType->IrTypeMade())
        {
            classType->MakeIrType();
        }
        WriteIrLayout(classType, codeFormatter);
        if (classType->IsVirtual())
        {
            WriteVtbl(classType, codeFormatter, externalFunctions, irFunctionRepository);
        }
    }
}

void LlvmIrClassTypeRepository::WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter)
{
    std::vector<Ir::Intf::Type*> memberTypes;
    std::vector<std::string> memberNames;
    std::string tagName = classType->GetMangleId() + "_";
    int index = 0;
    if (classType->BaseClass())
    {
        if (!classType->BaseClass()->IrTypeMade())
        {
            classType->BaseClass()->MakeIrType();
        }
        memberTypes.push_back(classType->BaseClass()->GetIrType()->Clone());
        memberNames.push_back("__base");
        ++index;
    }
    if (classType->MemberVariables().empty())
    {
        if (classType->VPtrIndex() != -1)
        {
            memberTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2));
            memberNames.push_back("__vptr");
        }
        else if (!classType->BaseClass())
        {
            memberTypes.push_back(Cm::IrIntf::I8());
            memberNames.push_back("__dummy");
        }
    }
    else
    {
        for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
        {
            if (index == classType->VPtrIndex())
            {
                memberTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 2));
                memberNames.push_back("__vptr");
                ++index;
            }
            Cm::Sym::TypeSymbol* memberVarType = memberVariable->GetType();
            if (!memberVarType->IrTypeMade())
            {
                memberVarType->MakeIrType();
            }
            if (!memberVarType->GetBaseType()->IrTypeMade())
            {
                memberVarType->GetBaseType()->MakeIrType();
            }
            if (memberVarType->IsPureArrayType())
            {
                Ir::Intf::Type* memberVarArrayType = Cm::IrIntf::Array(memberVarType->GetBaseType()->GetIrType(), memberVarType->GetLastArrayDimension());
                memberTypes.push_back(memberVarArrayType);
            }
            else
            {
                memberTypes.push_back(memberVarType->GetIrType()->Clone());
            }
            memberNames.push_back(memberVariable->Name());
            memberVariable->SetLayoutIndex(index);
            ++index;
        }
    }
	std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
	std::string typeDeclaration = classType->GetIrType()->Name() + " = type " + irTypeDeclaration->Name();
	codeFormatter.WriteLine(typeDeclaration);
}

void LlvmIrClassTypeRepository::WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter, 
    std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository)
{
    if (!classType->IsVirtual()) return;
    std::unique_ptr<Ir::Intf::Type> i8ptrType(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1));
    i8ptrType->SetOwned();
    std::unique_ptr<Ir::Intf::Type> vtblIrType(Cm::IrIntf::Array(i8ptrType->Clone(), int(classType->Vtbl().size())));
    vtblIrType->SetOwned();
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    std::unique_ptr<Ir::Intf::Object> vtblIrObject(Cm::IrIntf::CreateGlobal(vtblName, vtblIrType.get()));
    vtblIrObject->SetOwned();
    std::unique_ptr<Ir::Intf::Object> classNameIrValue(Cm::IrIntf::CreateStringConstant(classType->FullName()));
    classNameIrValue->SetOwned();
    std::unique_ptr<Ir::Intf::Object> classNameIrObject(Cm::IrIntf::CreateGlobal(Cm::IrIntf::MakeClassNameAssemblyName(classType->FullName()), Cm::IrIntf::Pointer(classNameIrValue->GetType(), 1)));
    classNameIrObject->SetOwned();
    codeFormatter.WriteLine(classNameIrObject->Name() + " = linkonce_odr unnamed_addr constant " + classNameIrValue->GetType()->Name() + " " + classNameIrValue->Name());

    std::string classRttiName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "rtti");
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(i8ptrType->Clone());
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    std::unique_ptr<Ir::Intf::Type> rttiIrType(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames));
    rttiIrType->SetOwned();
    std::unique_ptr<Ir::Intf::Object> rttiIrObject(Cm::IrIntf::CreateGlobal(classRttiName, rttiIrType.get()));
    rttiIrObject->SetOwned();
    if (Cm::Core::GetGlobalSettings()->LlvmVersion() < Cm::Ast::ProgramVersion::ProgramVersion(3, 7, 0, 0, ""))
    {
        codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " +
            std::to_string(classType->Cid()) + "}");
    }
    else
    {
        codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrValue->GetType()->Name() + ", " + 
            classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " + std::to_string(classType->Cid()) + "}");
    }

    std::string vtblHeader;
    vtblHeader.append(vtblIrObject->Name()).append(" = linkonce_odr unnamed_addr constant ").append(vtblIrType->Name());
    codeFormatter.WriteLine(vtblHeader);
    codeFormatter.WriteLine("[");
    codeFormatter.IncIndent();
    bool first = true;
    for (Cm::Sym::FunctionSymbol* virtualFunction : classType->Vtbl())
    {
        std::string line;
        line.append(i8ptrType->Name()).append(" ");
        if (first)
        {
            line.append("bitcast (%rtti* " + rttiIrObject->Name() + " to i8*)");
            first = false;
        }
        else
        {
            codeFormatter.WriteLine(",");
            if (!virtualFunction)
            {
                throw std::runtime_error("virtual function not set");
            }
            if (virtualFunction->IsAbstract())
            {
                line.append("null");
            }
            else
            {
                Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(virtualFunction);
                externalFunctions.insert(irFun);
                Ir::Intf::Type* irFunPtrType = irFunctionRepository.GetFunPtrIrType(virtualFunction);
                line.append("bitcast (").append(irFunPtrType->Name()).append(" @").append(irFun->Name()).append(" to ").append(i8ptrType->Name()).append(")");
            }
        }
        codeFormatter.Write(line);
    }
    codeFormatter.WriteLine();
    codeFormatter.DecIndent();
    codeFormatter.WriteLine("]");
}

void LlvmIrClassTypeRepository::WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter)
{
    codeFormatter.WriteLine("%destruction$node = type { %destruction$node*, i8*, void (i8*)* }");
    codeFormatter.WriteLine("declare void @register$destructor(%destruction$node*)");
}

void Visit(std::vector<std::string>& classOrder, const std::string& classTypeName, std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& tempVisit, std::unordered_map<std::string, std::vector<std::string>>& dependencyMap)
{
    if (tempVisit.find(classTypeName) == tempVisit.end())
    {
        if (visited.find(classTypeName) == visited.end())
        {
            tempVisit.insert(classTypeName);
            std::unordered_map<std::string, std::vector<std::string>>::const_iterator i = dependencyMap.find(classTypeName);
            if (i != dependencyMap.end())
            {
                const std::vector<std::string>& dependents = i->second;
                for (const std::string& dependent : dependents)
                {
                    Visit(classOrder, dependent, visited, tempVisit, dependencyMap);
                }
                tempVisit.erase(classTypeName);
                visited.insert(classTypeName);
                classOrder.push_back(classTypeName);
            }
            else
            {
                throw std::runtime_error("class type '" + classTypeName + "' not found in dependencies");
            }
        }
    }
    else
    {
        throw std::runtime_error("circular class dependency for class '" + classTypeName + "' detected");
    }
}

std::vector<Cm::Sym::ClassTypeSymbol*> CreateClassOrder(const std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>& classMap, std::unordered_map<std::string, std::vector<std::string>>& dependencyMap)
{
    std::vector<std::string> classNameOrder;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> tempVisit;
    for (const std::pair<std::string, std::vector<std::string>>& p : dependencyMap)
    {
        std::string classTypeName = p.first;
        if (visited.find(classTypeName) == visited.end())
        {
            Visit(classNameOrder, classTypeName, visited, tempVisit, dependencyMap);
        }
    }
    std::vector<Cm::Sym::ClassTypeSymbol*> classOrder;
    for (const std::string& className : classNameOrder)
    {
        std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>::const_iterator i = classMap.find(className);
        if (i != classMap.cend())
        {
            classOrder.push_back(i->second);
        }
        else
        {
            throw std::runtime_error("class name '" + className + "' not found in the class map");
        }
    }
    return classOrder;
}

void CIrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository, const std::vector<Ir::Intf::Type*>& tdfs)
{
    WriteDestructionNodeDef(codeFormatter);
    std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*> classMap;
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        Cm::Sym::ClassTypeSymbol* classType = p.second;
        if (!classType->IrTypeMade())
        {
            classType->MakeIrType();
        }
        classMap[classType->FullName()] = classType;
    }
    std::unordered_map<std::string, std::vector<std::string>> dependencyMap;
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        Cm::Sym::ClassTypeSymbol* classType = p.second;
        std::unordered_set<std::string> added;
        std::vector<std::string>& dependencies = dependencyMap[classType->FullName()];
        if (classType->BaseClass())
        {
            dependencies.push_back(classType->BaseClass()->FullName());
            added.insert(classType->BaseClass()->FullName());
        }
        for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
        {
            if (memberVariable->GetType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariable->GetType());
                if (added.find(memberVarClassType->FullName()) == added.end())
                {
                    added.insert(memberVarClassType->FullName());
                    dependencies.push_back(memberVarClassType->FullName());
                }
            }
        }
    }
    std::vector<Cm::Sym::ClassTypeSymbol*> classOrder = CreateClassOrder(classMap, dependencyMap);
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        std::unique_ptr<Ir::Intf::Type> typeName(Cm::IrIntf::CreateClassTypeName(classType->FullName()));
        codeFormatter.WriteLine("typedef struct " + classType->GetMangleId() + "_ " + typeName->Name() + ";");
    }
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        WriteIrLayout(classType, codeFormatter);
    }
    for (Ir::Intf::Type* tdf : tdfs)
    {
        codeFormatter.WriteLine(tdf->Name() + ";");
    }
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        WriteVtbl(classType, codeFormatter, externalFunctions, irFunctionRepository);
    }
}

void CIrClassTypeRepository::WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter)
{
    std::vector<Ir::Intf::Type*> memberTypes;
    std::vector<std::string> memberNames;
    std::string tagName = classType->GetMangleId() + "_";
    int index = 0;
    if (classType->BaseClass())
    {
        if (!classType->BaseClass()->IrTypeMade())
        {
            classType->BaseClass()->MakeIrType();
        }
        memberTypes.push_back(classType->BaseClass()->GetIrType()->Clone());
        memberNames.push_back("__base");
        ++index;
    }
    if (classType->MemberVariables().empty())
    {
        if (classType->VPtrIndex() != -1)
        {
            memberTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2));
            memberNames.push_back("__vptr");
        }
        else if (!classType->BaseClass())
        {
            memberTypes.push_back(Cm::IrIntf::I8());
            memberNames.push_back("__dummy");
        }
    }
    else
    {
        for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
        {
            Cm::Sym::TypeSymbol* memberVarType = memberVariable->GetType();
            if (!memberVarType->IrTypeMade())
            {
                memberVarType->MakeIrType();
            }
            if (!memberVarType->GetBaseType()->IrTypeMade())
            {
                memberVarType->GetBaseType()->MakeIrType();
            }
            if (index == classType->VPtrIndex())
            {
                memberTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2));
                memberNames.push_back("__vptr");
                ++index;
            }
            if (memberVarType->IsPureArrayType())
            {
                Ir::Intf::Type* memberVarArrayType = Cm::IrIntf::Array(memberVarType->GetBaseType()->GetIrType(), memberVarType->GetLastArrayDimension());
                memberTypes.push_back(memberVarArrayType);
            }
            else
            {
                memberTypes.push_back(memberVarType->GetIrType()->Clone());
            }
            memberNames.push_back(memberVariable->Name());
            memberVariable->SetLayoutIndex(index);
            ++index;
        }
    }
    std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
    std::unique_ptr<Ir::Intf::Type> typeName(Cm::IrIntf::CreateClassTypeName(classType->FullName()));
    codeFormatter.WriteLine("typedef " + irTypeDeclaration->Name() + " " + typeName->Name() + ";");
}

void CIrClassTypeRepository::WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
    std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository)
{
    if (!classType->IsVirtual()) return;
    for (Cm::Sym::FunctionSymbol* virtualFunction : classType->Vtbl())
    {
        if (virtualFunction)
        {
            Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(virtualFunction);
            irFun->WriteDeclaration(codeFormatter, virtualFunction->IsReplicated(), false);
        }
    }
    std::unique_ptr<Ir::Intf::Type> voidptrType(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1));
    voidptrType->SetOwned();
    std::unique_ptr<Ir::Intf::Type> vtblIrType(Cm::IrIntf::Array(voidptrType->Clone(), int(classType->Vtbl().size())));
    vtblIrType->SetOwned();
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    std::unique_ptr<Ir::Intf::Object> vtblIrObject(Cm::IrIntf::CreateGlobal(vtblName, vtblIrType.get()));
    vtblIrObject->SetOwned();

    std::string classRttiName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "rtti");
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::Char(), 1));
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    std::unique_ptr<Ir::Intf::Type> rttiIrType(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames));
    rttiIrType->SetOwned();
    std::unique_ptr<Ir::Intf::Object> rttiIrObject(Cm::IrIntf::CreateGlobal(classRttiName, rttiIrType.get()));
    rttiIrObject->SetOwned();
    codeFormatter.WriteLine("rtti " + classRttiName + " = {\"" + Cm::Util::StringStr(classType->FullName()) + "\", " + std::to_string(classType->Cid()) + "};");

    std::string vtblHeader = "static void* ";
    vtblHeader.append(vtblIrObject->Name()).append("[").append(std::to_string(classType->Vtbl().size())).append("] =");
    codeFormatter.WriteLine(vtblHeader);
    codeFormatter.WriteLine("{");
    codeFormatter.IncIndent();
    bool first = true;
    for (Cm::Sym::FunctionSymbol* virtualFunction : classType->Vtbl())
    {
        std::string line;
        if (first)
        {
            line.append("&").append(rttiIrObject->Name());
            first = false;
        }
        else
        {
            codeFormatter.WriteLine(",");
            if (!virtualFunction)
            {
                throw std::runtime_error("virtual function not set");
            }
            if (virtualFunction->IsAbstract())
            {
                line.append("(void*)0");
            }
            else
            {
                Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(virtualFunction);
                externalFunctions.insert(irFun);
                line.append("&").append(irFun->Name());
            }
        }
        codeFormatter.Write(line);
    }
    codeFormatter.WriteLine();
    codeFormatter.DecIndent();
    codeFormatter.WriteLine("};");
}

void CIrClassTypeRepository::WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter)
{
    codeFormatter.WriteLine("typedef struct destruction_X_node_ { struct destruction_X_node_* next; void* cls; void (*destructor)(void*); } destruction_X_node;");
    codeFormatter.WriteLine("void register_X_destructor(destruction_X_node* node);");
}

} } // namespace Cm::Core
