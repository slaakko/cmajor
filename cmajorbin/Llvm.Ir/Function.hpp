/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_FUNCTION_INCLUDED
#define LLVM_IR_FUNCTION_INCLUDED
#include <Ir.Intf/Function.hpp>

namespace Llvm { 

using Cm::Util::CodeFormatter;

extern const char* exceptionCodeParamName;
extern const char* classObjectResultParamName;

class Function : public Ir::Intf::Function
{
public:
    Function(const std::string& name_, Ir::Intf::Type* returnType_, const std::vector<Ir::Intf::Parameter*>& parameters_);
    ~Function();
    void AddInstruction(Ir::Intf::Instruction* instruction) override;
    void WriteDefinition(CodeFormatter& formatter, bool weakOdr, bool inline_) override;
    void WriteDeclaration(CodeFormatter& formatter, bool weakOdr, bool inline_) override;
    bool ContainsExceptionCodeParam() const override;
    bool IsDoNothingFunction() const override { return isDoNothingFunction; }
    void SetDoNothingFunction() { isDoNothingFunction = true; }
    bool IsMemSetFunction() const override { return isMemSetFunction; }
    void SetMemSetFunction() { isMemSetFunction = true; }
protected:
    std::string ParameterListStr() const override;
private:
    bool isDoNothingFunction;
    bool isMemSetFunction;
};

Ir::Intf::Function* CreateDoNothingFunction();
Ir::Intf::Function* CreateDbgDeclareFunction();
Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr);
Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr);

} // namespace Llvm

#endif // LLVM_IR_FUNCTION_INCLUDED
