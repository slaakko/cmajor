/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_C_EMITTER_INCLUDED
#define CM_EMIT_C_EMITTER_INCLUDED
#include <Cm.Emit/Emitter.hpp>
#include <Cm.Core/CDebugInfo.hpp>

namespace Cm { namespace Emit {

class CEmitter : public Emitter
{
public:
    CEmitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_);
    void WriteCompileUnitHeader(Cm::Util::CodeFormatter& codeFormatter) override;
    void BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit) override;
    void EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit) override;
    void BeginVisit(Cm::BoundTree::BoundClass& boundClass) override;
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
private:
    Cm::Core::CStaticMemberVariableRepository staticMemberVariableRepository;
    std::string funFilePath;
    std::ofstream funFile;
    std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*> functionMap;
    std::unique_ptr<Cm::Core::CDebugInfoFile> debugInfoFile;
    std::unique_ptr<Cm::Util::MappedInputFile> currentSourceFile;
    int funLine;
    std::string cFilePath;
    void GenerateDebugInfo(Cm::Sym::ClassTypeSymbol* classTypeSymbol);
};

} } // namespace Cm::Emit

#endif // CM_EMIT_C_EMITTER_INCLUDED
