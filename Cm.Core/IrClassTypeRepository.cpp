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
#include <Llvm.Ir/Type.hpp>
#include <stdexcept>

namespace Cm { namespace Core {

void IrClassTypeRepository::AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol)
{
    if (classTypes.find(classTypeSymbol) != classTypes.end()) return;
    classTypes.insert(classTypeSymbol);
    if (classTypeSymbol->BaseClass())
    {
        AddClassType(classTypeSymbol->BaseClass());
    }
    for (Cm::Sym::MemberVariableSymbol* memberVar : classTypeSymbol->MemberVariables())
    {
        Cm::Sym::TypeSymbol* memberVariableBaseType = memberVar->GetType()->GetBaseType();
        if (memberVariableBaseType->IsClassTypeSymbol())
        {
            AddClassType(static_cast<Cm::Sym::ClassTypeSymbol*>(memberVariableBaseType));
        }
    }
    if (classTypeSymbol->IsVirtual())
    {
        for (Cm::Sym::FunctionSymbol* virtualFunction : classTypeSymbol->Vtbl())
        {
            if (virtualFunction)
            {
                Cm::Sym::TypeSymbol* returnType = virtualFunction->GetReturnType();
                if (returnType && returnType->GetBaseType()->IsClassTypeSymbol())
                {
                    Cm::Sym::ClassTypeSymbol* returnClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(returnType->GetBaseType());
                    AddClassType(returnClassType);
                }
                for (Cm::Sym::ParameterSymbol* parameter : virtualFunction->Parameters())
                {
                    Cm::Sym::TypeSymbol* parameterBaseType = parameter->GetType()->GetBaseType();
                    if (parameterBaseType->IsClassTypeSymbol())
                    {
                        Cm::Sym::ClassTypeSymbol* parameterClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parameterBaseType);
                        AddClassType(parameterClassType);
                    }
                }
            }
        }
    }
}

void IrClassTypeRepository::Write(Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit, std::unordered_set<Ir::Intf::Function*>& externalFunctions, 
    IrFunctionRepository& irFunctionRepository)
{
    bool exitDeclarationsGenerated = false;
    for (Cm::Sym::ClassTypeSymbol* classType : classTypes)
    {
        WriteIrLayout(classType, codeFormatter);
        if (classType->IsVirtual())
        {
            WriteVtbl(classType, codeFormatter, syntaxUnit, externalFunctions, irFunctionRepository);
        }
        for (Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol : classType->StaticMemberVariables())
        {
            if (staticMemberVariableSymbol->GetType()->IsClassTypeSymbol() && static_cast<Cm::Sym::ClassTypeSymbol*>(staticMemberVariableSymbol->GetType())->Destructor() && !exitDeclarationsGenerated)
            {
                exitDeclarationsGenerated = true;
                codeFormatter.WriteLine("%destruction$node = type { %destruction$node*, i8*, void (i8*)* }");
                codeFormatter.WriteLine("declare void @register$destructor(%destruction$node*)");
            }
        }
    }
}

void IrClassTypeRepository::WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter)
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

void IrClassTypeRepository::WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter, Cm::Ast::CompileUnitNode* syntaxUnit, 
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

} } // namespace Cm::Core
