/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Inspect.hpp>
#include <Cm.Debugger/InspectExpr.hpp>
#include <Cm.Debugger/TypeExpr.hpp>
#include <Cm.Debugger/Result.hpp>
#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Debugger/Gdb.hpp>

namespace Cm { namespace Debugger {

bool HasReferenceOrRvalueRefDerivation(const Cm::Ast::DerivationList& derivations)
{
    for (Cm::Ast::Derivation derivation : derivations)
    {
        if (derivation == Cm::Ast::Derivation::reference || derivation == Cm::Ast::Derivation::rvalueRef)
        {
            return true;
        }
    }
    return false;
}

bool HasPointerDerivation(const Cm::Ast::DerivationList& derivations)
{
    for (Cm::Ast::Derivation derivation : derivations)
    {
        if (derivation == Cm::Ast::Derivation::pointer) return true;
    }
    return false;
}

PrintExpr::PrintExpr(const std::string& text_, TypeExpr* typeExpr_) : text(text_), typeExpr(typeExpr_)
{
}

void PrintExpr::SetTypeExpr(TypeExpr* typeExpr_)
{
    typeExpr.reset(typeExpr_);
}

void PrintExpr::SetDisplayTypeExpr(TypeExpr* displayTypeExpr_)
{
    displayTypeExpr.reset(displayTypeExpr_);
}

std::unique_ptr<Result> MakeErrorResult(const std::string& errorMessage, const std::string& name)
{
    std::unique_ptr<Result> errorResult(new Result(name, -1));
    errorResult->SetValue(new ErrorValue(errorMessage));
    return errorResult;
}

InspectExprGrammar* inspectExprGrammar = nullptr;

TypeExprGrammar* typeExprParser = nullptr;

ResultGrammar* resultGrammar = nullptr;

Inspector::Inspector(Gdb& gdb_, DebugInfo& debugInfo_, Cm::Core::CfgNode* currentNode_) : gdb(gdb_), debugInfo(debugInfo_), currentNode(currentNode_)
{
    if (!inspectExprGrammar)
    {
        inspectExprGrammar = InspectExprGrammar::Create();
    }
    if (!typeExprParser)
    {
        typeExprParser = TypeExprGrammar::Create();
    }
    if (!resultGrammar)
    {
        resultGrammar = ResultGrammar::Create();
    }
}

void Inspector::PushPrintExpr(PrintExpr&& printExpr)
{
    printExprStack.push(std::move(printExpr));
}

PrintExpr Inspector::PopPrintExpr()
{
    PrintExpr printExpr = std::move(printExprStack.top());
    printExprStack.pop();
    return printExpr;
}

void Inspector::Parse(const std::string& expr_)
{
    expr = expr_;
    inspectNode.reset(inspectExprGrammar->Parse(expr.c_str(), expr.c_str() + expr.length(), 0, ""));
    inspectNode->Accept(*this);
}

std::vector<std::unique_ptr<Result>> Inspector::GetResults()
{
    return std::move(results);
}

void Inspector::Visit(SingleNode& singleNode)
{
    PrintExpr printExpr = PopPrintExpr();
    std::shared_ptr<GdbCommand> command = gdb.Print(printExpr.Text());
    if (command->ReplyMessage().empty())
    {
        std::unique_ptr<Result> errorResult = MakeErrorResult("could not evaluate debugger expression '" + printExpr.Text() + "'", expr);
        results.push_back(std::move(errorResult));
    }
    else
    {
        std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", expr));
        result->SetType(printExpr.GetTypeExpr()->ToString());
        result->SetDisplayType(printExpr.GetDisplayTypeExpr()->ToString());
        results.push_back(std::move(result));
    }
}

void Inspector::Visit(ContentNode& contentNode)
{
    PrintExpr printExpr = PopPrintExpr();
    TypeExpr* typeExpr = printExpr.GetTypeExpr();
    if (HasReferenceOrRvalueRefDerivation(typeExpr->Derivations()))
    {
        printExpr.Text() = "(*" + printExpr.Text() + ")";
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        typeExpr->Derivations().RemoveReferenceOrRvalueRef();
    }
    else
    {
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    }
    if (typeExpr->Derivations().NumDerivations() == 0)
    {
        if (typeExpr->PrimaryTypeName() == "System.Collections.List")
        {
            InspectList(printExpr);
        }
        else if (typeExpr->PrimaryTypeName() == "System.Collections.Set")
        {
            InspectSet(printExpr);
        }
        else if (typeExpr->PrimaryTypeName() == "System.Collections.Map")
        {
            InspectMap(printExpr);
        }
        else
        {
            InspectClass(printExpr, typeExpr);
        }
    }
    else if (HasPointerDerivation(typeExpr->Derivations()))
    {
        InspectPointer(printExpr, typeExpr);
    }
    else
    {
        throw std::runtime_error("unknown derivations for expression '" + printExpr.Text() + "'");
    }
}

void Inspector::Visit(LocalNode& localNode)
{
    Cm::Core::CFunctionDebugInfo* currentFunction = currentNode->Function();
    Cm::Core::Local* local = currentFunction->Locals().GetLocal(localNode.LocalVarName());
    if (!local)
    {
        throw std::runtime_error("local variable '" + localNode.LocalVarName() + "' not found");
    }
    std::unordered_set<int32_t> nodeSet = currentFunction->Cfg().GetNodeSetIncluding(currentNode->Id());
    if (local->DefNode() == currentNode->Id() || nodeSet.find(local->DefNode()) == nodeSet.end())
    {
        throw std::runtime_error("local variable '" + localNode.LocalVarName() + "' not initialized yet");
    }
    const std::string& irName = local->IrName();
    TypeExpr* typeExpr = typeExprParser->Parse(local->TypeName().c_str(), local->TypeName().c_str() + local->TypeName().length(), 0, "");
    PrintExpr printExpr(irName, typeExpr);
    if (HasReferenceOrRvalueRefDerivation(typeExpr->Derivations()))
    {
        printExpr.Text() = "(*" + printExpr.Text() + ")";
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        typeExpr->Derivations().RemoveReferenceOrRvalueRef();
    }
    else
    {
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    }
    PushPrintExpr(std::move(printExpr));
}

void Inspector::Visit(HandleNode& handleNode)
{
    TypeExpr* typeExpr = typeExprParser->Parse(handleNode.TypeExpr().c_str(), handleNode.TypeExpr().c_str() + handleNode.TypeExpr().length(), 0, "");
    std::string handleExpr = "$" + std::to_string(handleNode.Handle());
    PrintExpr printExpr(handleExpr, typeExpr);
    if (HasReferenceOrRvalueRefDerivation(typeExpr->Derivations()))
    {
        printExpr.Text() = "(*" + printExpr.Text() + ")";
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        typeExpr->Derivations().RemoveReferenceOrRvalueRef();
    }
    else
    {
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    }
    PushPrintExpr(std::move(printExpr));
}

void Inspector::Visit(DerefNode& derefNode)
{
    PrintExpr printExpr = PopPrintExpr();
    TypeExpr* typeExpr = printExpr.GetTypeExpr();
    if (!HasPointerDerivation(typeExpr->Derivations()))
    {
        throw std::runtime_error("type of expression '" + printExpr.Text() + "' does not have pointer derivation");
    }
    typeExpr->Derivations().RemoveLastPointer();
    printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    printExpr.Text() = "*" + printExpr.Text();
    PushPrintExpr(std::move(printExpr));
}

void Inspector::Visit(DotNode& dotNode)
{
    PrintExpr printExpr = PopPrintExpr();
    TypeExpr* typeExpr = printExpr.GetTypeExpr();
    if (HasReferenceOrRvalueRefDerivation(typeExpr->Derivations()))
    {
        printExpr.Text() = "(*" + printExpr.Text() + ")";
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        typeExpr->Derivations().RemoveReferenceOrRvalueRef();
    }
    else
    {
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    }
    std::string classTypeName = typeExpr->ToString();
    std::string memberTypeName;
    Cm::Core::ClassDebugInfo* classDebugInfo = debugInfo.GetClassDebugInfo(classTypeName);
    if (classDebugInfo)
    {
        if (dotNode.MemberId() == "base")
        {
            if (!classDebugInfo->BaseClassFullName().empty())
            {
                memberTypeName = classDebugInfo->BaseClassFullName();
                printExpr.Text() = printExpr.Text() + ".__base";
            }
            else
            {
                throw std::runtime_error("base class for '" + classTypeName + "' not found");
            }
        }
        else
        {
            Cm::Core::MemberVariableDebugInfo* memberVarDebugInfo = classDebugInfo->GetMemberVariable(dotNode.MemberId());
            if (memberVarDebugInfo)
            {
                memberTypeName = memberVarDebugInfo->MemberVarTypeName();
                printExpr.Text() = printExpr.Text() + "." + dotNode.MemberId();
            }
            else
            {
                throw std::runtime_error("member variable '" + dotNode.MemberId() + "' not found in class '" + classTypeName + "'");
            }
        }
        TypeExpr* typeExpr = typeExprParser->Parse(memberTypeName.c_str(), memberTypeName.c_str() + memberTypeName.length(), 0, "");
        printExpr.SetTypeExpr(typeExpr);
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        PushPrintExpr(std::move(printExpr));
    }
    else
    {
        throw std::runtime_error("class debug info for '" + classTypeName + "' not found");
    }
}

void Inspector::Visit(ArrowNode& arrowNode)
{
    PrintExpr printExpr = PopPrintExpr();
    TypeExpr* typeExpr = printExpr.GetTypeExpr();
    if (HasReferenceOrRvalueRefDerivation(typeExpr->Derivations()))
    {
        printExpr.Text() = "(*" + printExpr.Text() + ")";
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        typeExpr->Derivations().RemoveReferenceOrRvalueRef();
    }
    else
    {
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
    }
    if (!HasPointerDerivation(typeExpr->Derivations()))
    {
        throw std::runtime_error("type of expression '" + printExpr.Text() + "' does not have pointer derivation");
    }
    std::unique_ptr<TypeExpr> withOutPtr(typeExpr->Clone());
    withOutPtr->Derivations().RemoveLastPointer();
    std::string classTypeName = withOutPtr->ToString();
    std::string memberTypeName;
    Cm::Core::ClassDebugInfo* classDebugInfo = debugInfo.GetClassDebugInfo(classTypeName);
    if (classDebugInfo)
    {
        if (arrowNode.MemberId() == "base")
        {
            if (!classDebugInfo->BaseClassFullName().empty())
            {
                memberTypeName = classDebugInfo->BaseClassFullName();
                printExpr.Text() = printExpr.Text() + "->__base";
            }
            else
            {
                throw std::runtime_error("base class for '" + classTypeName + "' not found");
            }
        }
        else
        {
            Cm::Core::MemberVariableDebugInfo* memberVarDebugInfo = classDebugInfo->GetMemberVariable(arrowNode.MemberId());
            if (memberVarDebugInfo)
            {
                memberTypeName = memberVarDebugInfo->MemberVarTypeName();
                printExpr.Text() = printExpr.Text() + "->" + arrowNode.MemberId();
            }
            else
            {
                throw std::runtime_error("member variable '" + arrowNode.MemberId() + "' not found in class '" + classTypeName + "'");
            }
        }
        TypeExpr* typeExpr = typeExprParser->Parse(memberTypeName.c_str(), memberTypeName.c_str() + memberTypeName.length(), 0, "");
        printExpr.SetTypeExpr(typeExpr);
        printExpr.SetDisplayTypeExpr(typeExpr->Clone());
        PushPrintExpr(std::move(printExpr));
    }
    else
    {
        throw std::runtime_error("class debug info for '" + classTypeName + "' not found");
    }
}

void Inspector::Visit(ParenthesesNode& parenthesesNode)
{
    PrintExpr printExpr = PopPrintExpr();
    printExpr.Text() = "(" + printExpr.Text() + ")";
    printExprStack.push(std::move(printExpr));
}

void Inspector::InspectClass(const PrintExpr& printExpr, TypeExpr* typeExpr)
{
    std::string classTypeName = typeExpr->ToString();
    Cm::Core::ClassDebugInfo* classDebugInfo = debugInfo.GetClassDebugInfo(classTypeName);
    if (classDebugInfo)
    {
        if (!classDebugInfo->BaseClassFullName().empty())
        {
            TypeExpr* baseClassTypeExpr = typeExprParser->Parse(classDebugInfo->BaseClassFullName().c_str(), classDebugInfo->BaseClassFullName().c_str() + classDebugInfo->BaseClassFullName().length(), 0, "");
            PrintExpr baseClassPrintExpr(printExpr.Text() + ".__base", baseClassTypeExpr);
            baseClassPrintExpr.SetDisplayTypeExpr(baseClassTypeExpr->Clone());
            std::shared_ptr<GdbCommand> command = gdb.Print(baseClassPrintExpr.Text());
            if (command->ReplyMessage().empty())
            {
                std::unique_ptr<Result> errorResult = MakeErrorResult("could not evaluate debugger expression '" + baseClassPrintExpr.Text() + "'", "[" + classDebugInfo->BaseClassFullName() + "]");
                results.push_back(std::move(errorResult));
            }
            else
            {
                std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "",
                    "[" + classDebugInfo->BaseClassFullName() + "]"));
                result->SetType(baseClassPrintExpr.GetTypeExpr()->ToString());
                result->SetDisplayType(baseClassPrintExpr.GetDisplayTypeExpr()->ToString());
                results.push_back(std::move(result));
            }
        }
        for (const Cm::Core::MemberVariableDebugInfo& memberVar : classDebugInfo->MemberVariables())
        {
            TypeExpr* memberVarTypeExpr = typeExprParser->Parse(memberVar.MemberVarTypeName().c_str(), memberVar.MemberVarTypeName().c_str() + memberVar.MemberVarTypeName().length(), 0, "");
            PrintExpr memberVarPrintExpr(printExpr.Text() + "." + memberVar.MemberVarName(), memberVarTypeExpr);
            if (HasReferenceOrRvalueRefDerivation(memberVarTypeExpr->Derivations()))
            {
                memberVarPrintExpr.Text() = "(*" + memberVarPrintExpr.Text() + ")";
                memberVarPrintExpr.SetDisplayTypeExpr(memberVarTypeExpr->Clone());
                memberVarTypeExpr->Derivations().RemoveReferenceOrRvalueRef();
            }
            else
            {
                memberVarPrintExpr.SetDisplayTypeExpr(memberVarTypeExpr->Clone());
            }
            std::shared_ptr<GdbCommand> command = gdb.Print(memberVarPrintExpr.Text());
            if (command->ReplyMessage().empty())
            {
                std::unique_ptr<Result> errorResult = MakeErrorResult("could not evaluate debugger expression '" + memberVarPrintExpr.Text() + "'", memberVar.MemberVarName());
                results.push_back(std::move(errorResult));
            }
            else
            {
                std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", memberVar.MemberVarName()));
                result->SetType(memberVarPrintExpr.GetTypeExpr()->ToString());
                result->SetDisplayType(memberVarPrintExpr.GetDisplayTypeExpr()->ToString());
                results.push_back(std::move(result));
            }
        }
    }
    else
    {
        throw std::runtime_error("class '" + classTypeName + "' not found");
    }
}

void Inspector::InspectPointer(const PrintExpr& printExpr, TypeExpr* typeExpr)
{
    typeExpr->Derivations().RemoveLastPointer();
    PrintExpr derefExpr("(*" + printExpr.Text() + ")", typeExpr->Clone());
    if (typeExpr->PrimaryTypeName() == "System.Collections.List")
    {
        InspectList(derefExpr);
    }
    else if (typeExpr->PrimaryTypeName() == "System.Collections.Set")
    {
        InspectSet(derefExpr);
    }
    else if (typeExpr->PrimaryTypeName() == "System.Collections.Map")
    {
        InspectMap(derefExpr);
    }
    else
    {
        if (typeExpr->IsBasicTypeExpr())
        {
            std::shared_ptr<GdbCommand> command = gdb.Print(derefExpr.Text());
            if (command->ReplyMessage().empty())
            {
                std::unique_ptr<Result> errorResult = MakeErrorResult("could not evaluate debugger expression '" + derefExpr.Text() + "'", "*");
                results.push_back(std::move(errorResult));
            }
            else
            {
                std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", "*"));
                result->SetType(typeExpr->ToString());
                result->SetDisplayType(typeExpr->ToString());
                results.push_back(std::move(result));
            }
        }
        else
        {
            std::string classTypeName = typeExpr->ToString();
            Cm::Core::ClassDebugInfo* classDebugInfo = debugInfo.GetClassDebugInfo(classTypeName);
            if (classDebugInfo)
            {
                if (!classDebugInfo->IsVirtual())
                {
                    InspectClass(derefExpr, typeExpr);
                }
                else
                {
                    InspectVirtualClass(classDebugInfo, printExpr, derefExpr, typeExpr);
                }
            }
            else
            {
                throw std::runtime_error("debug info for class '" + classTypeName + "' not found");
            }
        }
    }
}

void Inspector::InspectVirtualClass(Cm::Core::ClassDebugInfo* classDebugInfo, const PrintExpr& printExpr, const PrintExpr& derefExpr, TypeExpr* typeExpr)
{
    std::string actualClassName = GetActualClassName(classDebugInfo, derefExpr);
    Cm::Core::ClassDebugInfo* actualClassDebugInfo = debugInfo.GetClassDebugInfo(actualClassName);
    if (actualClassDebugInfo)
    {
        TypeExpr* actualTypeExpr = typeExprParser->Parse(actualClassName.c_str(), actualClassName.c_str() + actualClassName.length(), 0, "");
        PrintExpr actualClassExpr("*(" + actualClassDebugInfo->IrTypeName() + "*)" + printExpr.Text(), actualTypeExpr);
        std::shared_ptr<GdbCommand> command = gdb.Print(actualClassExpr.Text());
        if (!command->ReplyMessage().empty())
        {
            std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", "[" + actualClassName + "]"));
            result->SetType(actualClassName);
            result->SetDisplayType(actualClassName);
            results.push_back(std::move(result));
        }
        else
        {
            std::unique_ptr<Result> result = MakeErrorResult("class '" + actualClassName + "' does not exist in current context", "[" + actualClassName + "]");
            results.push_back(std::move(result));
            InspectClass(derefExpr, typeExpr);
        }
    }
    else
    {
        throw std::runtime_error("debug info for class '" + actualClassName + "' not found");
    }
}

std::string Inspector::GetActualClassName(Cm::Core::ClassDebugInfo* classDebugInfo, const PrintExpr& printExpr)
{
    if (classDebugInfo->HasVptr())
    {
        std::string printClassNameExpr = "*(char**)" + printExpr.Text() + ".__vptr";
        std::shared_ptr<GdbCommand> command = gdb.Print(printClassNameExpr);
        if (command->ReplyMessage().empty())
        {
            throw std::runtime_error("could not get actual class name for class '" + classDebugInfo->FullName() + "'");
        }
        std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", "className"));
        Value* value = result->GetValue();
        if (value->IsStringValue())
        {
            StringValue* s = static_cast<StringValue*>(value);
            return s->Value();
        }
        else
        {
            throw std::runtime_error("string value expected");
        }
    }
    else if (!classDebugInfo->BaseClassFullName().empty())
    {
        TypeExpr* baseClassTypeExpr = typeExprParser->Parse(classDebugInfo->BaseClassFullName().c_str(), classDebugInfo->BaseClassFullName().c_str() + classDebugInfo->BaseClassFullName().length(), 0, "");
        std::string baseClassTypeName = baseClassTypeExpr->ToString();
        Cm::Core::ClassDebugInfo* baseClassDebugInfo = debugInfo.GetClassDebugInfo(baseClassTypeName);
        if (baseClassDebugInfo)
        {
            PrintExpr baseExpr("(" + printExpr.Text() + ".__base)", baseClassTypeExpr);
            return GetActualClassName(baseClassDebugInfo, baseExpr);
        }
        else
        {
            throw std::runtime_error("debug info for class '" + baseClassTypeName + "' not found");
        }
    }
    else
    {
        throw std::runtime_error("vptr container not found for class '" + classDebugInfo->FullName() + "'");
    }
}

void Inspector::InspectList(const PrintExpr& printExpr)
{
    TypeExpr* type = printExpr.GetTypeExpr();
    if (type->TypeArguments().size() != 1)
    {
        throw std::runtime_error("number of type arguments in System.List is not 1");
    }
    TypeExpr* itemType = type->TypeArguments()[0].get();
    std::shared_ptr<GdbCommand> command = gdb.Print(printExpr.Text());
    if (command->ReplyMessage().empty())
    {
        throw std::runtime_error("could not evaluate debugger expression '" + printExpr.Text() + "'");
    }
    std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", "System.List"));
    if (result->GetValue()->IsStructureValue())
    {
        StructureValue* listStruct = static_cast<StructureValue*>(result->GetValue());
        Value* countValue = listStruct->GetFieldValue("count");
        if (!countValue)
        {
            throw std::runtime_error("count field not found in System.List");
        }
        if (countValue->IsIntegerValue())
        {
            IntegerValue* count = static_cast<IntegerValue*>(countValue);
            if (count->IsNegative())
            {
                throw std::runtime_error("count is negative");
            }
            uint64_t n = count->AbsoluteValue();
            if (n == 0)
            {
                std::unique_ptr<Result> result(new Result("count", -1));
                result->SetValue(new IntegerValue(0, false));
                result->SetType("int");
                result->SetDisplayType("int");
                results.push_back(std::move(result));
            }
            else
            {
                for (uint64_t i = 0; i < n; ++i)
                {
                    PrintExpr itemExpr(printExpr.Text() + ".items[" + std::to_string(i) + "]", itemType->Clone());
                    std::shared_ptr<GdbCommand> command = gdb.Print(itemExpr.Text());
                    if (command->ReplyMessage().empty())
                    {
                        throw std::runtime_error("could not evaluate item expression '" + itemExpr.Text() + "'");
                    }
                    std::unique_ptr<Result> result(resultGrammar->Parse(command->ReplyMessage().c_str(), command->ReplyMessage().c_str() + command->ReplyMessage().length(), 0, "", "[" + std::to_string(i) + "]"));
                    result->SetType(itemType->ToString());
                    result->SetDisplayType(itemType->ToString());
                    results.push_back(std::move(result));
                }
            }
        }
        else
        {
            throw std::runtime_error("count of System.List not integer");
        }
    }
    else
    {
        throw std::runtime_error("structure expected");
    }
}

void Inspector::InspectSet(const PrintExpr& printExpr)
{

}

void Inspector::InspectMap(const PrintExpr& printExpr)
{

}

} } // Cm::Debugger
