/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_EMITTER_VISITOR_INCLUDED
#define CM_EMIT_EMITTER_VISITOR_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Core/StaticMemberVariableRepository.hpp>
#include <Cm.Core/ExternalConstantRepository.hpp>
#include <fstream>

namespace Cm { namespace Emit {

class EmittingVisitor : public Cm::BoundTree::Visitor
{
public:
    EmittingVisitor(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, Cm::Core::IrClassTypeRepository& irClassTypeRepository_, 
        Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_);
    void BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit) override;
    void EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit) override;
    void BeginVisit(Cm::BoundTree::BoundClass& boundClass) override;
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
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
    std::unordered_set<Ir::Intf::Function*> externalFunctions;
    Cm::Core::StaticMemberVariableRepository staticMemberVariableRepository;
};

} } // namespace Cm::Emit

#endif // CM_EMIT_EMITTER_VISITOR_INCLUDED
