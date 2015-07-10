/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Function.hpp>
#include <C.Ir/Instruction.hpp>
#include <C.Ir/Parameter.hpp>
#include <C.Ir/Factory.hpp>
#include <Ir.Intf/Metadata.hpp>

namespace C {

const char* exceptionCodeParamName = "_X_ex_p";
const char* classObjectResultParamName = "_X_result";

Function::Function(const std::string& name_, Ir::Intf::Type* returnType_, const std::vector<Ir::Intf::Parameter*>& parameters_) : Ir::Intf::Function(name_, returnType_, parameters_)
{
}

Function::~Function()
{
}

std::string Function::ParameterListStr() const
{
    std::string parameterList("(");
    if (Parameters().empty())
    {
        parameterList.append("void");
    }
    else
    {
        bool first = true;
        for (Ir::Intf::Parameter* parameter : Parameters())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                parameterList.append(", ");
            }
            Ir::Intf::Type* paramType = parameter->GetType();
            if (paramType->IsFunctionPointerType() || paramType->IsFunctionPtrPtrType())
            {
                parameterList.append(paramType->Prefix() + parameter->Name() + paramType->Suffix());
            }
            else
            {
                parameterList.append(paramType->Name() + " " + parameter->Name());
            }
        }
    }
    parameterList.append(")");
    return parameterList;
}

bool Function::ContainsExceptionCodeParam() const
{
    return !Parameters().empty() && Parameters().back()->Name() == std::string("_P_") + exceptionCodeParamName;
}

void Function::AddInstruction(Ir::Intf::Instruction* instruction)
{
    Ir::Intf::Object* result = instruction->ReturnsResult() ? instruction->GetResult() : nullptr;
    if (result)
    {
        if (result->Name() == "_R_?")
        {
            result->SetName("_R_" + std::to_string(GetNextInstNumber()));
            registers.push_back(result);
        }
    }
    DoAddInstruction(instruction);
    Own(instruction);
}

void Function::WriteDeclaration(CodeFormatter& formatter, bool weakOdr, bool inline_)
{
    std::string declaration;
    if (inline_)
    {
        declaration.append("static inline ");
    }
    declaration.append(GetReturnType()->Name()).append(" ").append(Name()).append(ParameterListStr()).append(";");
    formatter.WriteLine(declaration);
}

void Function::WriteDefinition(CodeFormatter& formatter, bool weakOdr, bool inline_)
{
    formatter.WriteLine();
    if (!Comment().empty())
    {
        formatter.WriteLine("// " + Comment());
    }
    std::string define;
    if (inline_)
    {
        define.append("static inline ");
    }
    define.append(GetReturnType()->Name()).append(" ").append(Name()).append(ParameterListStr());
    formatter.WriteLine(define);
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (Ir::Intf::Object* r : registers)
    {
        Ir::Intf::Type* regType = r->GetType();
        if (regType->IsFunctionPointerType() || regType->IsFunctionPtrPtrType())
        {
            formatter.WriteLine(regType->Prefix() + r->Name() + regType->Suffix() + ";");
        }
        else
        {
            formatter.WriteLine(regType->Name() + " " + r->Name() + ";");
        }
    }
    Ir::Intf::Instruction* prev = nullptr;
    for (Ir::Intf::Instruction* inst : Instructions())
    {
        if (inst->Removed())
        {
            continue;
        }
        Ir::Intf::LabelObject* label = inst->GetLabel();
        if (label || prev && prev->IsTerminator())
        {
            if (label)
            {
                if (prev && !prev->IsTerminator())
                {
                    std::unique_ptr<Ir::Intf::Instruction> br(Br(label));
                    formatter.WriteLine(br->ToString() + ";");
                }
            }
            else
            {
                label = CreateNextLocalLabel();
                Own(label);
            }
            formatter.DecIndent();
            formatter.WriteLine();
            formatter.WriteLine(label->Target());
            formatter.IncIndent();
        }
        std::string s = inst->ToString();
        s.append(";");
        const std::string& comment = inst->Comment();
        if (!comment.empty())
        {
            s.append(" // ").append(comment);
        }
        Ir::Intf::CDebugNode* cDebugNode = inst->GetCDebugNode();
        if (cDebugNode)
        {
            cDebugNode->SetCLine(formatter.Line());
        }
        formatter.WriteLine(s);
        Ir::Intf::CDebugNode* funCallNode = inst->GetFunCallNode();
        if (funCallNode)
        {
            funCallNode->SetCLine(formatter.Line());
            formatter.WriteLine("_V__X_dummy = true;");
        }
        prev = inst;
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr)
{
    std::vector<Ir::Intf::Parameter*> parameters;
    parameters.push_back(new Parameter("dest", i8Ptr));
    parameters.push_back(new Parameter("value", Ir::Intf::GetFactory()->GetI8()));
    parameters.push_back(new Parameter("len", Ir::Intf::GetFactory()->GetI64()));
    parameters.push_back(new Parameter("align", Ir::Intf::GetFactory()->GetI32()));
    parameters.push_back(new Parameter("isVolatile", Ir::Intf::GetFactory()->GetI1()));
    Function* memSetFun(new Function("cmemset", Ir::Intf::GetFactory()->GetVoid(), parameters));
    return memSetFun;
}

Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr)
{
    std::vector<Ir::Intf::Parameter*> parameters;
    parameters.push_back(new Parameter("dest", i8Ptr));
    parameters.push_back(new Parameter("source", i8Ptr));
    parameters.push_back(new Parameter("len", Ir::Intf::GetFactory()->GetI64()));
    parameters.push_back(new Parameter("align", Ir::Intf::GetFactory()->GetI32()));
    parameters.push_back(new Parameter("isVolatile", Ir::Intf::GetFactory()->GetI1()));
    Function* memSetFun(new Function("cmemcpy", Ir::Intf::GetFactory()->GetVoid(), parameters));
    return memSetFun;
}

} // namespace C
