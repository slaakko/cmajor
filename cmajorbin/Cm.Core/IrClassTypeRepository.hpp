/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
#define CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.IrIntf/Fwd.hpp>

namespace Cm { namespace Core {

class IrClassTypeRepository : public Cm::Sym::BcuItem
{ 
public:
    virtual ~IrClassTypeRepository();
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    bool Added(Cm::Sym::ClassTypeSymbol* classType) const;
    void AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol);
    virtual void Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
        IrFunctionRepository& irFunctionRepository) = 0;
    virtual void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) = 0;
    virtual void WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) = 0;
    void SetLayoutIndeces();
    virtual void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) = 0;
    const std::unordered_set<Cm::Sym::ClassTypeSymbol*>& ClassTypes() const { return classTypes; }
    std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>& ClassTypeMap() { return classTypeMap; }
private:
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> classTypes;
    std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*> classTypeMap;
};

class LlvmIrClassTypeRepository : public IrClassTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
        IrFunctionRepository& irFunctionRepository) override;
    void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) override;
    void WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) override;
    void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) override;
};

class CIrClassTypeRepository : public IrClassTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
        IrFunctionRepository& irFunctionRepository) override;
    void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) override;
    void WriteVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) override;
    void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) override;
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
