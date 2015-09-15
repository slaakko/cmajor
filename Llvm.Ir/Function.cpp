/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Function.hpp>
#include <Llvm.Ir/Metadata.hpp>
#include <Llvm.Ir/Instruction.hpp>
#include <Llvm.Ir/Parameter.hpp>
#include <Llvm.Ir/Factory.hpp>
#include <Llvm.Ir/Type.hpp>

namespace Llvm { 

const char* exceptionCodeParamName = "$ex$p";
const char* classObjectResultParamName = "$cls$result$p";

Function::Function(const std::string& name_, Ir::Intf::Type* returnType_, const std::vector<Ir::Intf::Parameter*>& parameters_): Ir::Intf::Function(name_, returnType_, parameters_), 
    isDoNothingFunction(false), isMemSetFunction(false)
{
}

Function::~Function()
{
}

std::string Function::ParameterListStr() const
{
    std::string parameterList("(");
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
        parameterList.append(parameter->GetType()->Name() + " " + parameter->Name());
    }
    parameterList.append(")");
    return parameterList;
}

bool Function::ContainsExceptionCodeParam() const
{
    return !Parameters().empty() && Parameters().back()->Name() == std::string("%") + exceptionCodeParamName;
}

void Function::AddInstruction(Ir::Intf::Instruction* instruction)
{
    Ir::Intf::Object* result = instruction->ReturnsResult() ? instruction->GetResult() : nullptr;
    if (result)
    {
        if (result->Name() == "%%")
        {
            result->SetName("%" + std::to_string(GetNextInstNumber()));
        }
    }
    DoAddInstruction(instruction);
    Own(instruction);
}

void Function::WriteDeclaration(CodeFormatter& formatter, bool weakOdr, bool inline_)
{
    std::string declaration("declare ");
    declaration.append(GetReturnType()->Name()).append(" @").append(Name()).append(ParameterListStr()).append(" nounwind");
    formatter.WriteLine(declaration);
}

void Function::WriteDefinition(CodeFormatter& formatter, bool weakOdr, bool inline_)
{
    formatter.WriteLine();
    if (!Comment().empty())
    {
        formatter.WriteLine("; " + Comment());
    }
    std::string define("define ");
    if (inline_ || weakOdr)
    {
        define.append("weak_odr ");
    }
    define.append(GetReturnType()->Name()).append(" @").append(Name()).append(ParameterListStr()).append(" nounwind");
    if (inline_)
    {
        define.append(" inlinehint");
    }
    formatter.WriteLine(define);
    formatter.WriteLine("{");
    formatter.IncIndent();
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
                    formatter.WriteLine(br->ToString());
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
        Ir::Intf::MetadataNode* sourcePosNode = inst->GetSourcePosMetadataNode();
        if (sourcePosNode)
        {
            s.append(", !dbg !").append(std::to_string(sourcePosNode->Id()));
        }
        const std::string& comment = inst->Comment();
        if (!comment.empty())
        {
            s.append(" ; ").append(comment);
        }
        formatter.WriteLine(s);
        prev = inst;
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

Ir::Intf::Function* CreateDoNothingFunction()
{
    Function* doNothingFun(new Function("llvm.donothing", Ir::Intf::GetFactory()->GetVoid(), std::vector<Ir::Intf::Parameter*>()));
    doNothingFun->SetDoNothingFunction();
    return doNothingFun;
}

Ir::Intf::Function* CreateDbgDeclareFunction()
{
    std::vector<Ir::Intf::Parameter*> parameters;
    Ir::Intf::Type* metadataType = Ir::Intf::GetFactory()->GetMetadataType();
    parameters.push_back(new Parameter("variable", metadataType));
    parameters.push_back(new Parameter("description", metadataType));
    Ir::Intf::Function* dbgDeclareFun(new Function("llvm.dbg.declare", Ir::Intf::GetFactory()->GetVoid(), parameters));
    return dbgDeclareFun;
}

Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr)
{
    std::vector<Ir::Intf::Parameter*> parameters;
    parameters.push_back(new Parameter("dest", i8Ptr));
    parameters.push_back(new Parameter("value", Ir::Intf::GetFactory()->GetI8()));
    parameters.push_back(new Parameter("len", Ir::Intf::GetFactory()->GetI64()));
    parameters.push_back(new Parameter("align", Ir::Intf::GetFactory()->GetI32()));
    parameters.push_back(new Parameter("isVolatile", Ir::Intf::GetFactory()->GetI1()));
    Function* memSetFun(new Function("llvm.memset.p0i8.i64", Ir::Intf::GetFactory()->GetVoid(), parameters));
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
    Function* memSetFun(new Function("llvm.memcpy.p0i8.p0i8.i64", Ir::Intf::GetFactory()->GetVoid(), parameters));
    return memSetFun;
}

} // namespace Llvm
