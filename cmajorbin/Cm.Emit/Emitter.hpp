/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_EMITTER_INCLUDED
#define CM_EMIT_EMITTER_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Core/StaticMemberVariableRepository.hpp>
#include <Cm.Core/ExternalConstantRepository.hpp>
#include <Cm.Core/GenData.hpp>
#include <fstream>

namespace Cm { namespace Emit {

class Emitter : public Cm::BoundTree::Visitor
{
public:
    Emitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, Cm::Core::IrClassTypeRepository& irClassTypeRepository_,
        Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_);
    void BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit) override;
    virtual void WriteCompileUnitHeader(Cm::Util::CodeFormatter& codeFormatter) = 0;
    virtual Cm::Core::StaticMemberVariableRepository& GetStaticMemberVariableRepository() = 0;
protected:
    const std::unordered_set<Ir::Intf::Function*>& ExternalFunctions() const { return externalFunctions; }
    std::unordered_set<Ir::Intf::Function*>& ExternalFunctions() { return externalFunctions; }
    const std::unordered_set<std::string>& InternalFunctionNames() const { return internalFunctionNames; }
    std::unordered_set<std::string>& InternalFunctionNames() { return internalFunctionNames; }
    Cm::Util::CodeFormatter& CodeFormatter() { return codeFormatter; }
    Cm::Core::ExternalConstantRepository& ExternalConstantRepository() { return externalConstantRepository; }
    Cm::BoundTree::BoundClass* CurrentClass() { return currentClass; }
    void SetCurrentClass(Cm::BoundTree::BoundClass* currentClass_) { currentClass = currentClass_; }
    const std::unordered_set<Cm::Sym::ClassTypeSymbol*>& ProcessedClasses() const { return processedClasses; }
    std::unordered_set<Cm::Sym::ClassTypeSymbol*>& ProcessedClasses() { return processedClasses; }
    Cm::Sym::TypeRepository& TypeRepository() { return typeRepository; }
    Cm::Core::IrFunctionRepository& IrFunctionRepository() { return irFunctionRepository; }
    Cm::Core::IrClassTypeRepository& IrClassTypeRepository() { return irClassTypeRepository; }
    Cm::Core::StringRepository& StringRepository() { return stringRepository; }
    Cm::Ast::CompileUnitNode* CurrentCompileUnit() { return currentCompileUnit; }
    Cm::Sym::FunctionSymbol* EnterFrameFun() { return enterFrameFun; }
    Cm::Sym::FunctionSymbol* LeaveFrameFun() { return leaveFrameFun; }
    Cm::Sym::FunctionSymbol* EnterTracedCallFun() { return enterTracedCallFun; }
    Cm::Sym::FunctionSymbol* LeaveTracedCallFun() {return leaveTracedCallFun; }
    Cm::Sym::SymbolTable* SymbolTable() const { return symbolTable; }
    bool Profile() const { return profile; }
private:
	Cm::Sym::TypeRepository& typeRepository;
    Cm::Core::IrFunctionRepository& irFunctionRepository;
    Cm::Core::IrClassTypeRepository& irClassTypeRepository;
    Cm::Core::StringRepository& stringRepository;
    Cm::Core::ExternalConstantRepository& externalConstantRepository;
    std::ofstream irFile;
    Cm::Util::CodeFormatter codeFormatter;
    Cm::BoundTree::BoundClass* currentClass;
    Cm::Ast::CompileUnitNode* currentCompileUnit;
    std::unordered_set<std::string> internalFunctionNames;
    std::unordered_set<Ir::Intf::Function*> externalFunctions;
	Cm::Sym::FunctionSymbol* enterFrameFun;
	Cm::Sym::FunctionSymbol* leaveFrameFun;
    Cm::Sym::FunctionSymbol* enterTracedCallFun;
    Cm::Sym::FunctionSymbol* leaveTracedCallFun;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> processedClasses;
    Cm::Sym::SymbolTable* symbolTable;
    bool profile;
};

} } // namespace Cm::Emit

#endif // CM_EMIT_EMITTER_INCLUDED
