/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_FUNCTION_INCLUDED
#define IR_INTF_FUNCTION_INCLUDED
#include <Ir.Intf/Instruction.hpp>
#include <Ir.Intf/Parameter.hpp>
#include <Cm.Util/CodeFormatter.hpp>

namespace Ir { namespace Intf {

using Cm::Util::CodeFormatter;

class Function;

class Emitter
{
public:
    virtual ~Emitter() {}
    virtual void Emit(Instruction* instruction) = 0;
    virtual void Own(Object* object) = 0;
    virtual void Own(Type* type) = 0;
    virtual void Own(Function* fun) = 0;
};

class TempTypedefProvider
{
public:
    virtual std::string GetNextTempTypedefName() = 0;
};

TempTypedefProvider* GetCurrentTempTypedefProvider();
void SetCurrentTempTypedefProvider(TempTypedefProvider* tempTypedefProvider);

class Function
{
public:
    Function(const std::string& name_, Type* returnType_, const std::vector<Parameter*>& parameters_);
    virtual ~Function();
    const std::string& Name() const { return name; }
    Type* ReturnType() const { return returnType; }
    virtual void AddInstruction(Instruction* instruction) = 0;
    void Own(Instruction* instruction);
    void Own(Object* object);
    void Own(Function* fun);
    void SetLastInstructionLabel(LabelObject* label);
    void Clean();
    virtual void WriteDefinition(CodeFormatter& formatter, bool weakOdr, bool inline_) = 0;
    void WriteInlineDefinition(CodeFormatter& formatter);
    virtual void WriteDeclaration(CodeFormatter& formatter, bool weakOdr, bool inline_) = 0;
    virtual void ReplaceFunctionPtrTypes() {}
    virtual bool ContainsExceptionCodeParam() const = 0;
    virtual bool IsDoNothingFunction() const { return false; }
    virtual bool IsMemSetFunction() const { return false; }
    bool LastInstructionIsRet() const;
    void SetComment(const std::string& comment_) { comment = comment_; }
    void Release();
    const std::vector<Parameter*>& Parameters() const { return parameters; }
    Type* GetReturnType() const { return returnType; }
protected:
    void DoAddInstruction(Instruction* instruction);
    int GetNextInstNumber();
    virtual std::string ParameterListStr() const = 0;
    const std::string& Comment() const { return comment; }
    const std::vector<Instruction*>& Instructions() const { return instructions; }
private:
    std::string name;
    Type* returnType;
    std::vector<Parameter*> parameters;
    std::vector<Instruction*> instructions;
    std::vector<std::unique_ptr<Instruction>> ownedInstructions;
    std::vector<std::unique_ptr<Object>> ownedObjects;
    std::vector<std::unique_ptr<Function>> ownedFunctions;
    std::vector<std::unique_ptr<Parameter>> ownedParameters;
    int instNumber;
    std::string comment;
};

} } // namespace Llvm::Ir

#endif // IR_INTF_FUNCTION_INCLUDED
