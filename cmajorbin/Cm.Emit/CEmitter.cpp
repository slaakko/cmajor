/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/CEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Emit/CFunctionEmitter.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Util/Path.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace Emit {

CEmitter::CEmitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Emitter(irFilePath, typeRepository_, irFunctionRepository_, irClassTypeRepository_, stringRepository_, externalConstantRepository_), funLine(1), symbolTable(nullptr)
{
}

void CEmitter::WriteCompileUnitHeader(Cm::Util::CodeFormatter& codeFormatter)
{
    codeFormatter.WriteLine("#include <stdint.h>");
    codeFormatter.WriteLine("#define null 0");
    codeFormatter.WriteLine("#define true 1");
    codeFormatter.WriteLine("#define false 0");
    codeFormatter.WriteLine("typedef _Bool i1;");
    codeFormatter.WriteLine("typedef int8_t i8;");
    codeFormatter.WriteLine("typedef uint8_t ui8;");
    codeFormatter.WriteLine("typedef int16_t i16;");
    codeFormatter.WriteLine("typedef uint16_t ui16;");
    codeFormatter.WriteLine("typedef int32_t i32;");
    codeFormatter.WriteLine("typedef uint32_t ui32;");
    codeFormatter.WriteLine("typedef int64_t i64;");
    codeFormatter.WriteLine("typedef uint64_t ui64;");
}

void CEmitter::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    Emitter::BeginVisit(compileUnit);
    funFilePath = Cm::Util::GetFullPath(boost::filesystem::path(compileUnit.IrFilePath()).replace_extension(".fun").generic_string());
    funFile.open(funFilePath.c_str());
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_debug_info) && !compileUnit.IsMainUnit())
    {
        debugInfoFile.reset(new Cm::Core::CDebugInfoFile());
        currentSourceFile.reset(new Cm::Util::MappedInputFile(compileUnit.SyntaxUnit()->FilePath()));
    }
    symbolTable = &compileUnit.SymbolTable();
    cFilePath = compileUnit.IrFilePath();
}

void CEmitter::EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    for (Ir::Intf::Function* function : ExternalFunctions())
    {
        std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*>::iterator i = functionMap.find(function);
        bool isInline = false;
        if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::optimize))
        {
            if (i != functionMap.end())
            {
                Cm::Sym::FunctionSymbol* fun = i->second;
                isInline = fun->IsInline();
            }
        }
        function->WriteDeclaration(CodeFormatter(), false, isInline);
    }
    staticMemberVariableRepository.Write(CodeFormatter());
    ExternalConstantRepository().Write(CodeFormatter());
    funFile.close();
    if (debugInfoFile)
    {
        debugInfoFile->FixCLines(CodeFormatter().Line() - 1);
    }
    std::string funFileContent(Cm::Util::ReadFile(funFilePath));
    CodeFormatter().WriteLine(funFileContent);
    boost::filesystem::remove(funFilePath);
    if (debugInfoFile)
    {
        Cm::Ser::BinaryWriter writer(compileUnit.CDebugInfoFilePath());
        debugInfoFile->Write(writer);
    }
}

void CEmitter::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
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

void CEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsExternal()) return;
    Cm::Util::CodeFormatter funFormatter(funFile);
    funFormatter.SetLine(funLine);
    const char* start = nullptr;
    const char* end = nullptr;
    if (debugInfoFile)
    {
        if (!currentSourceFile)
        {
            throw std::runtime_error("current source file not set");
        }
        start = currentSourceFile->Begin();
        end = currentSourceFile->End();
    }
    CFunctionEmitter functionEmitter(funFormatter, TypeRepository(), IrFunctionRepository(), IrClassTypeRepository(), StringRepository(), CurrentClass(), InternalFunctionNames(),
        ExternalFunctions(), staticMemberVariableRepository, ExternalConstantRepository(), CurrentCompileUnit(), EnterFrameFun(), LeaveFrameFun(), EnterTracedCallFun(), LeaveTracedCallFun(),
        start, end, debugInfoFile != nullptr);
    functionEmitter.SetFunctionMap(&functionMap);
    functionEmitter.SetSymbolTable(symbolTable);
    functionEmitter.SetCFilePath(cFilePath);
    boundFunction.Accept(functionEmitter);
    funLine = funFormatter.Line();
    if (debugInfoFile)
    {
        Cm::Core::CFunctionDebugInfo* functionDebugInfo = functionEmitter.ReleaseFunctionDebugInfo();
        if (functionDebugInfo)
        {
            debugInfoFile->AddFunctionDebugInfo(functionDebugInfo);
        }
        else
        {
            throw std::runtime_error("has no function debug info");
        }
    }
}

} } // namespace Cm::Emit
