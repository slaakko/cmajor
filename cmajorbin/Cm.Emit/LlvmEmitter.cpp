/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/LlvmEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Emit/LlvmFunctionEmitter.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Util/Path.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace Emit {

std::string Unquote(const std::string& s)
{
    std::string t;
    for (char c : s)
    {
        if (c != '"')
        {
            t.append(1, c);
        }
    }
    return t;
}

LlvmEmitter::LlvmEmitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::IrInterfaceTypeRepository& irInterfaceTypeRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Emitter(irFilePath, typeRepository_, irFunctionRepository_, irClassTypeRepository_, irInterfaceTypeRepository_, stringRepository_, externalConstantRepository_)
{
}

void LlvmEmitter::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Emitter::BeginVisit(compileUnit);
    funFilePath = Cm::Util::GetFullPath(boost::filesystem::path(compileUnit.IrFilePath()).replace_extension(".fun").generic_string());
    funFile.open(funFilePath.c_str());
}

void LlvmEmitter::WriteCompileUnitHeader(Cm::Util::CodeFormatter& codeFormatter)
{
    std::string targetTriple = Unquote(Cm::Core::GetGlobalSettings()->TargetTriple());
    if (!targetTriple.empty())
    {
        codeFormatter.WriteLine("target triple = \"" + targetTriple + "\"");
    }
    std::string dataLayout = Unquote(Cm::Core::GetGlobalSettings()->Datalayout());
    if (!dataLayout.empty())
    {
        codeFormatter.WriteLine("target datalayout = \"" + dataLayout + "\"");
    }
    codeFormatter.WriteLine("%irec = type { i64, i8* }");
    codeFormatter.WriteLine("%rtti = type { i8*, i64, %irec* }");
}

struct FunctionNameLess
{
    inline bool operator()(Ir::Intf::Function* left, Ir::Intf::Function* right) const
    {
        return left->Name() < right->Name();
    }
};

void LlvmEmitter::EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    IrClassTypeRepository().Write(CodeFormatter(), ExternalFunctions(), IrFunctionRepository(), std::vector<Ir::Intf::Type*>());
    IrInterfaceTypeRepository().Write(CodeFormatter());
    std::vector<Ir::Intf::Function*> ef;
    for (Ir::Intf::Function* function : ExternalFunctions())
    {
        ef.push_back(function);
    }
    std::sort(ef.begin(), ef.end(), FunctionNameLess());
    for (Ir::Intf::Function* function : ef)
    {
        if (InternalFunctionNames().find(function->Name()) == InternalFunctionNames().end())
        {
            function->WriteDeclaration(CodeFormatter(), false, false);
        }
    }
    staticMemberVariableRepository.Write(CodeFormatter());
    ExternalConstantRepository().Write(CodeFormatter());
    funFile.close();
    std::string funFileContent(Cm::Util::ReadFile(funFilePath));
    CodeFormatter().WriteLine(funFileContent);
    boost::filesystem::remove(funFilePath);
}

void LlvmEmitter::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    SetCurrentClass(&boundClass);
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = boundClass.Symbol();
    if (ProcessedClasses().find(classTypeSymbol) != ProcessedClasses().end()) return;
    ProcessedClasses().insert(classTypeSymbol);
    for (Cm::Sym::MemberVariableSymbol* staticMemberVarSymbol : classTypeSymbol->StaticMemberVariables())
    {
        staticMemberVariableRepository.Add(staticMemberVarSymbol);
    }
    if (classTypeSymbol->InitializedVar())
    {
        staticMemberVariableRepository.Add(classTypeSymbol->InitializedVar());
    }
}

void LlvmEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsExternal()) return;
    Cm::Util::CodeFormatter funFormatter(funFile);
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig))
    {
        if (generatedFunctions.find(boundFunction.GetFunctionSymbol()->FullName()) != generatedFunctions.cend()) return;
        generatedFunctions.insert(boundFunction.GetFunctionSymbol()->FullName());
    }
    LlvmFunctionEmitter functionEmitter(funFormatter, TypeRepository(), IrFunctionRepository(), IrClassTypeRepository(), StringRepository(), CurrentClass(), InternalFunctionNames(),
        ExternalFunctions(), staticMemberVariableRepository, ExternalConstantRepository(), CurrentCompileUnit(), EnterFrameFun(), LeaveFrameFun(), EnterTracedCallFun(), LeaveTracedCallFun(), 
        InterfaceLookupFailed(), Profile());
    functionEmitter.SetSymbolTable(SymbolTable());
    functionEmitter.SetTpGraph(TpGraph());
    boundFunction.Accept(functionEmitter);
}

} } // namespace Cm::Emit
