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
#include <Cm.Sym/InterfaceTypeSymbol.hpp>
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

void IrClassTypeRepository::Own(Ir::Intf::Type* type)
{
    if (!type->Owned())
    {
        type->SetOwned();
        ownedTypes.push_back(std::unique_ptr<Ir::Intf::Type>(type));
    }
}

void IrClassTypeRepository::Own(Ir::Intf::Object* object)
{
    if (!object->Owned())
    {
        object->SetOwned();
        ownedObjects.push_back(std::unique_ptr<Ir::Intf::Object>(object));
    }
}

struct ClassNameLess
{
    inline bool operator()(Cm::Sym::ClassTypeSymbol* left, Cm::Sym::ClassTypeSymbol* right) const
    {
        return left->FullName() < right->FullName();
    }
};

void LlvmIrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository, 
    const std::vector<Ir::Intf::Type*>& tdfs)
{
    WriteDestructionNodeDef(codeFormatter);
    std::vector<Cm::Sym::ClassTypeSymbol*> cm;
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        cm.push_back(p.second);
    }
    std::sort(cm.begin(), cm.end(), ClassNameLess());
    for (Cm::Sym::ClassTypeSymbol* p : cm)
    {
        Cm::Sym::ClassTypeSymbol* classType = p;
        if (!classType->IrTypeMade())
        {
            classType->MakeIrType();
        }
        WriteIrLayout(classType, codeFormatter);
        if (classType->IsVirtual())
        {
            WriteItbvlsAndVtbl(classType, codeFormatter, externalFunctions, irFunctionRepository);
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

void LlvmIrClassTypeRepository::WriteItbvlsAndVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter, 
    std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository)
{
    if (!classType->IsVirtual()) return;
    std::unique_ptr<Ir::Intf::Type> i8ptrType(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1));
    i8ptrType->SetOwned();
    std::vector<std::string> iids;
    std::vector<Ir::Intf::Type*> itabIrTypes;
    std::vector<std::string> itabNames;
    for (const Cm::Sym::ITable& itab : classType->ITabs())
    {
        iids.push_back(std::to_string(itab.Intf()->Iid()));
        Ir::Intf::Type* itabIrType(Cm::IrIntf::Array(i8ptrType->Clone(), int(itab.IntfMemFunImpl().size())));
        Own(itabIrType);
        itabIrTypes.push_back(itabIrType);
        std::string itabName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + itab.Intf()->FullName() + Cm::IrIntf::GetPrivateSeparator() + "itab");
        itabNames.push_back("@" + itabName);
        Ir::Intf::Object* itabIrObject(Cm::IrIntf::CreateGlobal(itabName, itabIrType));
        Own(itabIrObject);
        codeFormatter.WriteLine(itabIrObject->Name() + " = linkonce_odr unnamed_addr constant " + itabIrType->Name());
        codeFormatter.WriteLine("[");
        codeFormatter.IncIndent();
        bool first = true;
        for (Cm::Sym::FunctionSymbol* memFun : itab.IntfMemFunImpl())
        {
            std::string line;
            if (first)
            {
                first = false;
            }
            else
            {
                codeFormatter.WriteLine(",");
            }
            Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(memFun);
            externalFunctions.insert(irFun);
            Ir::Intf::Type* irFunPtrType = irFunctionRepository.GetFunPtrIrType(memFun);
            line.append("i8* bitcast (").append(irFunPtrType->Name()).append(" @").append(irFun->Name()).append(" to ").append(i8ptrType->Name()).append(")");
            codeFormatter.Write(line);
        }
        codeFormatter.WriteLine();
        codeFormatter.DecIndent();
        codeFormatter.WriteLine("]");
    }
    std::string irecTabName;
    Ir::Intf::Type* irecTabIrType = nullptr;
    Ir::Intf::Object* irecTabIrObject = nullptr;
    if (!classType->ITabs().empty())
    {
        std::vector<Ir::Intf::Type*> irecElementTypes;
        std::vector<std::string> irecElementNames;
        irecElementTypes.push_back(Cm::IrIntf::UI64());
        irecElementNames.push_back("iid");
        irecElementTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::I8(), 1));
        irecElementNames.push_back("itab");
        int n = int(classType->ITabs().size()) + 1;
        irecTabIrType = Cm::IrIntf::Array(Cm::IrIntf::CreateTypeName("irec", false), n);
        Own(irecTabIrType);
        irecTabName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "irectab");
        irecTabIrObject = Cm::IrIntf::CreateGlobal(irecTabName, irecTabIrType);
        Own(irecTabIrObject);
        codeFormatter.WriteLine(irecTabIrObject->Name() + " = linkonce_odr unnamed_addr constant " + irecTabIrType->Name());
        codeFormatter.WriteLine("[");
        codeFormatter.IncIndent();
        int nitabs = int(classType->ITabs().size());
        for (int i = 0; i < nitabs; ++i)
        {
            if (i > 0)
            {
                codeFormatter.WriteLine(",");
            }
            std::string irec = "%irec { ";
            irec.append(Ir::Intf::GetFactory()->GetUI64()->Name()).append(" ").append(iids[i]).append(", ").append("i8* bitcast (").append(itabIrTypes[i]->Name()).append("* ").append(itabNames[i]).append(" to i8*)").append(" }");
            codeFormatter.Write(irec);
        }
        codeFormatter.WriteLine(",");
        std::string irec = "%irec { i64 " + Ir::Intf::GetFactory()->GetUI64()->CreateMinusOne()->Name() + ", i8* null }";
        codeFormatter.WriteLine(irec);
        codeFormatter.DecIndent();
        codeFormatter.WriteLine("]");
    }
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
    Ir::Intf::Type* irecTabType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 1);
    rttiElementTypes.push_back(irecTabType);
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    rttiElementNames.push_back("irectab");
    std::unique_ptr<Ir::Intf::Type> rttiIrType(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames));
    rttiIrType->SetOwned();
    std::unique_ptr<Ir::Intf::Object> rttiIrObject(Cm::IrIntf::CreateGlobal(classRttiName, rttiIrType.get()));
    rttiIrObject->SetOwned();
    if (Cm::Core::GetGlobalSettings()->LlvmVersion() < Cm::Ast::ProgramVersion(3, 7, 0, 0, ""))
    {
        if (irecTabName.empty())
        {
            codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " +
                std::to_string(classType->Cid()) + ", %irec* null }");
        }
        else
        {
            Ir::Intf::Type* irecTabIrObjectPtrType = Cm::IrIntf::Pointer(irecTabIrObject->GetType(), 1);
            Own(irecTabIrObjectPtrType);
            codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " +
                std::to_string(classType->Cid()) + ", %irec* getelementptr (" + irecTabIrObjectPtrType->Name() + " " + irecTabIrObject->Name() + ", i32 0, i32 0)");
        }
    }
    else
    {
        if (irecTabName.empty())
        {
            codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrValue->GetType()->Name() + ", " +
                classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " + std::to_string(classType->Cid()) + ", %irec* null }");
        }
        else
        {
            Ir::Intf::Type* irecTabIrObjectPtrType = Cm::IrIntf::Pointer(irecTabIrObject->GetType(), 1);
            Own(irecTabIrObjectPtrType);
            codeFormatter.WriteLine(rttiIrObject->Name() + " = linkonce_odr unnamed_addr constant %rtti { i8* getelementptr (" + classNameIrValue->GetType()->Name() + ", " +
                classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0), i64 " + std::to_string(classType->Cid()) + 
                ", %irec* getelementptr (" + irecTabIrObject->GetType()->Name() + ", " + irecTabIrObjectPtrType->Name() + " " + irecTabIrObject->Name() + ", i32 0, i32 0)}");
        }
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

void Visit(std::vector<std::string>& classOrder, const std::string& classTypeName, std::set<std::string>& visited,
    std::set<std::string>& tempVisit, std::map<std::string, std::vector<std::string>>& dependencyMap)
{
    if (tempVisit.find(classTypeName) == tempVisit.end())
    {
        if (visited.find(classTypeName) == visited.end())
        {
            tempVisit.insert(classTypeName);
            std::map<std::string, std::vector<std::string>>::const_iterator i = dependencyMap.find(classTypeName);
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

std::vector<Cm::Sym::ClassTypeSymbol*> CreateClassOrder(const std::map<std::string, Cm::Sym::ClassTypeSymbol*>& classMap, std::map<std::string, std::vector<std::string>>& dependencyMap)
{
    std::vector<std::string> classNameOrder;
    std::set<std::string> visited;
    std::set<std::string> tempVisit;
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
        std::map<std::string, Cm::Sym::ClassTypeSymbol*>::const_iterator i = classMap.find(className);
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
    std::vector<Cm::Sym::ClassTypeSymbol*> cm;
    for (const std::pair<std::string, Cm::Sym::ClassTypeSymbol*>& p : ClassTypeMap())
    {
        cm.push_back(p.second);
    }
    std::sort(cm.begin(), cm.end(), ClassNameLess());
    std::map<std::string, Cm::Sym::ClassTypeSymbol*> classMap;
    for (Cm::Sym::ClassTypeSymbol* classType : cm)
    {
        if (!classType->IrTypeMade())
        {
            classType->MakeIrType();
        }
        classMap[classType->FullName()] = classType;
    }
    std::map<std::string, std::vector<std::string>> dependencyMap;
    for (Cm::Sym::ClassTypeSymbol* classType : cm)
    {
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
        WriteItbvlsAndVtbl(classType, codeFormatter, externalFunctions, irFunctionRepository);
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

void CIrClassTypeRepository::WriteItbvlsAndVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
    std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository)
{
    if (!classType->IsVirtual()) return;
    std::unique_ptr<Ir::Intf::Type> voidptrType(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 1));
    voidptrType->SetOwned();
    std::vector<std::string> iids;
    std::vector<Ir::Intf::Type*> itabIrTypes;
    std::vector<std::string> itabNames;
    for (const Cm::Sym::ITable& itab : classType->ITabs())
    {
        for (Cm::Sym::FunctionSymbol* memFun : itab.IntfMemFunImpl())
        {
            Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(memFun);
            irFun->WriteDeclaration(codeFormatter, memFun->IsReplicated(), false);
        }
        iids.push_back(std::to_string(itab.Intf()->Iid()));
        Ir::Intf::Type* itabIrType(Cm::IrIntf::Array(voidptrType->Clone(), int(itab.IntfMemFunImpl().size())));
        Own(itabIrType);
        itabIrTypes.push_back(itabIrType);
        std::string itabName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + itab.Intf()->FullName() + Cm::IrIntf::GetPrivateSeparator() + "itab");
        itabNames.push_back(itabName);
        Ir::Intf::Object* itabIrObject(Cm::IrIntf::CreateGlobal(itabName, itabIrType));
        Own(itabIrObject);
        std::string itabHeader = "static void* ";
        itabHeader.append(itabIrObject->Name()).append("[").append(std::to_string(itab.IntfMemFunImpl().size())).append("] =");
        codeFormatter.WriteLine(itabHeader);
        codeFormatter.WriteLine("{");
        codeFormatter.IncIndent();
        bool first = true;
        for (Cm::Sym::FunctionSymbol* memFun : itab.IntfMemFunImpl())
        {
            std::string line;
            if (first)
            {
                first = false;
            }
            else
            {
                codeFormatter.WriteLine(",");
            }
            Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(memFun);
            externalFunctions.insert(irFun);
            line.append("&").append(irFun->Name());
            codeFormatter.Write(line);
        }
        codeFormatter.WriteLine();
        codeFormatter.DecIndent();
        codeFormatter.WriteLine("};");
    }
    std::string irecTabName;
    if (!classType->ITabs().empty())
    {
        irecTabName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "irectab");
        std::string irecTabHeader = "static irec " + irecTabName + "[" + std::to_string(classType->ITabs().size() + 1) + "] =";
        codeFormatter.WriteLine(irecTabHeader);
        codeFormatter.WriteLine("{");
        codeFormatter.IncIndent();
        bool first = true;
        int index = 0;
        for (const Cm::Sym::ITable& itab : classType->ITabs())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                codeFormatter.WriteLine(",");
            }
            std::string line = "{ ";
            line.append(std::to_string(itab.Intf()->Iid())).append(", ").append(itabNames[index]);
            line.append(" }");
            codeFormatter.Write(line);
            ++index;
        }
        codeFormatter.WriteLine(",");
        std::string line = "{ ";
        line.append(Ir::Intf::GetFactory()->GetUI64()->CreateMinusOne()->Name()).append(", null");
        line.append(" }");
        codeFormatter.WriteLine(line);
        codeFormatter.DecIndent();
        codeFormatter.WriteLine("};");
    }
    for (Cm::Sym::FunctionSymbol* virtualFunction : classType->Vtbl())
    {
        if (virtualFunction)
        {
            Ir::Intf::Function* irFun = irFunctionRepository.CreateIrFunction(virtualFunction);
            irFun->WriteDeclaration(codeFormatter, virtualFunction->IsReplicated(), false);
        }
    }
    std::unique_ptr<Ir::Intf::Type> vtblIrType(Cm::IrIntf::Array(voidptrType->Clone(), int(classType->Vtbl().size())));
    vtblIrType->SetOwned();
    std::string vtblName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "vtbl");
    std::unique_ptr<Ir::Intf::Object> vtblIrObject(Cm::IrIntf::CreateGlobal(vtblName, vtblIrType.get()));
    vtblIrObject->SetOwned();
    std::string classRttiName = Cm::IrIntf::MakeAssemblyName(classType->FullName() + Cm::IrIntf::GetPrivateSeparator() + "rtti");
    std::vector<Ir::Intf::Type*> rttiElementTypes;
    rttiElementTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::Char(), 1));
    rttiElementTypes.push_back(Cm::IrIntf::UI64());
    Ir::Intf::Type* irecTabType = Cm::IrIntf::Pointer(Cm::IrIntf::CreateTypeName("irec", false), 1);
    rttiElementTypes.push_back(irecTabType);
    std::vector<std::string> rttiElementNames;
    rttiElementNames.push_back("class_name");
    rttiElementNames.push_back("class_id");
    rttiElementNames.push_back("irectab");
    std::unique_ptr<Ir::Intf::Type> rttiIrType(Cm::IrIntf::Structure("rtti_", rttiElementTypes, rttiElementNames));
    rttiIrType->SetOwned();
    std::unique_ptr<Ir::Intf::Object> rttiIrObject(Cm::IrIntf::CreateGlobal(classRttiName, rttiIrType.get()));
    rttiIrObject->SetOwned();
    if (irecTabName.empty())
    {
        codeFormatter.WriteLine("rtti " + classRttiName + " = {\"" + Cm::Util::StringStr(classType->FullName()) + "\", " + std::to_string(classType->Cid()) + ", null};");
    }
    else
    {
        codeFormatter.WriteLine("rtti " + classRttiName + " = {\"" + Cm::Util::StringStr(classType->FullName()) + "\", " + std::to_string(classType->Cid()) + ", " + irecTabName + "};");
    }
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
