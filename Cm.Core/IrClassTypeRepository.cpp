/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Llvm.Ir/Type.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

IrClassTypeRepository::~IrClassTypeRepository()
{
}

bool IrClassTypeRepository::Added(Cm::Sym::ClassTypeSymbol* classType) const
{
    return classTypes.find(classType) != classTypes.end();
}

void IrClassTypeRepository::AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    classTypes.insert(classTypeSymbol);
}

void LlvmIrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
    IrFunctionRepository& irFunctionRepository)
{
    bool exitDeclarationsGenerated = false;
    for (Cm::Sym::ClassTypeSymbol* classType : ClassTypes())
    {
        WriteIrLayout(classType, codeFormatter);
        if (classType->IsVirtual())
        {
            WriteVtbl(classType, codeFormatter, syntaxUnit, externalFunctions, irFunctionRepository);
        }
        for (Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol : classType->StaticMemberVariables())
        {
            if (staticMemberVariableSymbol->GetType()->IsClassTypeSymbol() && static_cast<Cm::Sym::ClassTypeSymbol*>(staticMemberVariableSymbol->GetType())->Destructor() && 
                !exitDeclarationsGenerated)
            {
                exitDeclarationsGenerated = true;
                WriteDestructionNodeDef(codeFormatter);
            }
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
            memberTypes.push_back(memberVariable->GetType()->GetIrType()->Clone());
            memberNames.push_back(memberVariable->Name());
            memberVariable->SetLayoutIndex(index);
            ++index;
        }
    }
	std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
	std::string typeDeclaration = classType->GetIrType()->Name() + " = type " + irTypeDeclaration->Name();
	codeFormatter.WriteLine(typeDeclaration);
}

void LlvmIrClassTypeRepository::WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit, 
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
            line.append("getelementptr (" + classNameIrObject->GetType()->Name() + " " + classNameIrObject->Name() + ", i32 0, i32 0)");
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

void Visit(std::vector<Cm::Sym::ClassTypeSymbol*>& classOrder, Cm::Sym::ClassTypeSymbol* classType, std::unordered_set<Cm::Sym::ClassTypeSymbol*>& visited,
    std::unordered_set<Cm::Sym::ClassTypeSymbol*>& tempVisit, std::unordered_map<Cm::Sym::ClassTypeSymbol*, std::vector<Cm::Sym::ClassTypeSymbol*>>& dependencyMap)
{
    if (tempVisit.find(classType) == tempVisit.end())
    {
        if (visited.find(classType) == visited.end())
        {
            tempVisit.insert(classType);
            std::unordered_map<Cm::Sym::ClassTypeSymbol*, std::vector<Cm::Sym::ClassTypeSymbol*>>::const_iterator i = dependencyMap.find(classType);
            if (i != dependencyMap.end())
            {
                const std::vector<Cm::Sym::ClassTypeSymbol*>& dependents = i->second;
                for (Cm::Sym::ClassTypeSymbol* dependent : dependents)
                {
                    Visit(classOrder, dependent, visited, tempVisit, dependencyMap);
                }
                tempVisit.erase(classType);
                visited.insert(classType);
                classOrder.push_back(classType);
            }
            else
            {
                throw std::runtime_error("class type '" + classType->FullName() + "' not found in dependencies");
            }
        }
    }
    else
    {
        throw std::runtime_error("circular class dependency for class '" + classType->FullName() + "' detected");
    }
}

std::vector<Cm::Sym::ClassTypeSymbol*> CreateClassOrder(std::unordered_map<Cm::Sym::ClassTypeSymbol*, std::vector<Cm::Sym::ClassTypeSymbol*>>& dependencyMap)
{
    std::vector<Cm::Sym::ClassTypeSymbol*> classOrder;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> visited;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> tempVisit;
    for (const std::pair<Cm::Sym::ClassTypeSymbol*, std::vector<Cm::Sym::ClassTypeSymbol*>> p : dependencyMap)
    {
        Cm::Sym::ClassTypeSymbol* classType = p.first;
        if (visited.find(classType) == visited.end())
        {
            Visit(classOrder, classType, visited, tempVisit, dependencyMap);
        }
    }
    return classOrder;
}

void CIrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
    IrFunctionRepository& irFunctionRepository)
{
    std::unordered_map<Cm::Sym::ClassTypeSymbol*, std::vector<Cm::Sym::ClassTypeSymbol*>> dependencyMap;
    for (Cm::Sym::ClassTypeSymbol* classType : ClassTypes())
    {
        std::unordered_set<Cm::Sym::ClassTypeSymbol*> added;
        std::vector<Cm::Sym::ClassTypeSymbol*>& dependencies = dependencyMap[classType];
        if (classType->BaseClass())
        {
            dependencies.push_back(classType->BaseClass());
            added.insert(classType->BaseClass());
        }
        for (Cm::Sym::MemberVariableSymbol* memberVariable : classType->MemberVariables())
        {
            if (memberVariable->GetType()->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* memberVarClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariable->GetType());
                if (added.find(memberVarClassType) == added.end())
                {
                    added.insert(memberVarClassType);
                    dependencies.push_back(memberVarClassType);
                }
            }
        }
    }
    std::vector<Cm::Sym::ClassTypeSymbol*> classOrder = CreateClassOrder(dependencyMap);
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        std::unique_ptr<Ir::Intf::Type> typeName(Cm::IrIntf::CreateClassTypeName(classType->FullName()));
        codeFormatter.WriteLine("typedef struct " + classType->GetMangleId() + "_ " + typeName->Name() + ";");
    }
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        WriteIrLayout(classType, codeFormatter);
    }
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        WriteVtbl(classType, codeFormatter, syntaxUnit, externalFunctions, irFunctionRepository);
    }
    bool exitDeclarationsGenerated = false;
    for (Cm::Sym::ClassTypeSymbol* classType : classOrder)
    {
        for (Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol : classType->StaticMemberVariables())
        {
            if (staticMemberVariableSymbol->GetType()->IsClassTypeSymbol() && static_cast<Cm::Sym::ClassTypeSymbol*>(staticMemberVariableSymbol->GetType())->Destructor() &&
                !exitDeclarationsGenerated)
            {
                exitDeclarationsGenerated = true;
                WriteDestructionNodeDef(codeFormatter);
            }
        }
    }
}

void CIrClassTypeRepository::WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter)
{
    if (!classType->Bound())
    {
        throw std::runtime_error("class type '" + classType->FullName() + "' not bound");
    }
    std::vector<Ir::Intf::Type*> memberTypes;
    std::vector<std::string> memberNames;
    std::string tagName = classType->GetMangleId() + "_";
    int index = 0;
    if (classType->BaseClass())
    {
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
            if (index == classType->VPtrIndex())
            {
                memberTypes.push_back(Cm::IrIntf::Pointer(Cm::IrIntf::Void(), 2));
                memberNames.push_back("__vptr");
                ++index;
            }
            memberTypes.push_back(memberVariable->GetType()->GetIrType()->Clone());
            memberNames.push_back(memberVariable->Name());
            memberVariable->SetLayoutIndex(index);
            ++index;
        }
    }
    std::unique_ptr<Ir::Intf::Type> irTypeDeclaration(Cm::IrIntf::Structure(tagName, memberTypes, memberNames));
    std::unique_ptr<Ir::Intf::Type> typeName(Cm::IrIntf::CreateClassTypeName(classType->FullName()));
    codeFormatter.WriteLine("typedef " + irTypeDeclaration->Name() + " " + typeName->Name() + ";");
}

void CIrClassTypeRepository::WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit,
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
            line.append("\"").append(Cm::Util::StringStr(classType->FullName())).append("\""); 
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
