/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
        IrFunctionRepository& irFunctionRepository, const std::vector<Ir::Intf::Type*>& tdfs) = 0;
    virtual void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) = 0;
    virtual void WriteItbvlsAndVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) = 0;
    void SetLayoutIndeces();
    virtual void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) = 0;
    std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*>& ClassTypeMap() { return classTypeMap; }
    void Own(Ir::Intf::Type* type);
    void Own(Ir::Intf::Object* object);
private:
    std::unordered_map<std::string, Cm::Sym::ClassTypeSymbol*> classTypeMap;
    std::vector<std::unique_ptr<Ir::Intf::Type>> ownedTypes;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
};

class LlvmIrClassTypeRepository : public IrClassTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
        IrFunctionRepository& irFunctionRepository, const std::vector<Ir::Intf::Type*>& tdfs) override;
    void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) override;
    void WriteItbvlsAndVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) override;
    void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) override;
};

class CIrClassTypeRepository : public IrClassTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& codeFormatter, std::unordered_set<Ir::Intf::Function*>& externalFunctions,
        IrFunctionRepository& irFunctionRepository, const std::vector<Ir::Intf::Type*>& tdfs) override;
    void WriteIrLayout(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter) override;
    void WriteItbvlsAndVtbl(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter,
        std::unordered_set<Ir::Intf::Function*>& externalFunctions, IrFunctionRepository& irFunctionRepository) override;
    void WriteDestructionNodeDef(Cm::Util::CodeFormatter& codeFormatter) override;
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
