/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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
    virtual void AddInstruction(Ir::Intf::Instruction* instruction);
    virtual void WriteDefinition(CodeFormatter& formatter, bool weakOdr, bool inline_);
    virtual void WriteDeclaration(CodeFormatter& formatter, bool weakOdr, bool inline_);
    virtual bool ContainsExceptionCodeParam() const;
    bool IsDoNothingFunction() const override { return isDoNothingFunction; }
    void SetDoNothingFunction() { isDoNothingFunction = true; }
protected:
    virtual std::string ParameterListStr() const;
private:
    bool isDoNothingFunction;
};

Ir::Intf::Function* CreateDoNothingFunction();
Ir::Intf::Function* CreateDbgDeclareFunction();

} // namespace Llvm

#endif // LLVM_IR_FUNCTION_INCLUDED
