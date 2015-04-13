/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef C_IR_FUNCTION_INCLUDED
#define C_IR_FUNCTION_INCLUDED
#include <Ir.Intf/Function.hpp>

namespace C {

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
protected:
    std::string ParameterListStr() const override;
private:
    std::vector<Ir::Intf::Object*> registers;
};

} // namespace C

#endif // C_IR_FUNCTION_INCLUDED
