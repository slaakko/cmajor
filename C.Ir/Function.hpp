/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef C_IR_FUNCTION_INCLUDED
#define C_IR_FUNCTION_INCLUDED
#include <C.Ir/Type.hpp>
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
    void ReplaceFunctionPtrTypes() override;
    bool ContainsExceptionCodeParam() const override;
    std::vector<std::unique_ptr<C::Typedef>> Tdfs() { return std::move(tdfs); }
protected:
    std::string ParameterListStr() const override;
private:
    std::vector<Ir::Intf::Object*> registers;
    std::vector<std::unique_ptr<C::Typedef>> tdfs;
    std::unordered_map<Ir::Intf::Type*, Ir::Intf::Type*> tdfMap;
    bool functionPtrTypesReplaced;
};

Ir::Intf::Function* CreateMemSetFunction(Ir::Intf::Type* i8Ptr);
Ir::Intf::Function* CreateMemCopyFunction(Ir::Intf::Type* i8Ptr);

} // namespace C

#endif // C_IR_FUNCTION_INCLUDED
