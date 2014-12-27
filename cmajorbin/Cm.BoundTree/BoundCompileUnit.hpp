/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
#define CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace BoundTree {

class BoundCompileUnit
{
public:
    BoundCompileUnit(const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_);
    const std::string& IrFilePath() const { return irFilePath; }
    const std::string& ObjectFilePath() const { return objectFilePath; }
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ConversionTable& ConversionTable() { return conversionTable; }
    Cm::Core::ClassConversionTable& ClassConversionTable() { return classConversionTable; }
    Cm::Core::IrFunctionRepository& IrFunctionRepository() { return irFunctionRepository; }
    void AddBoundNode(BoundNode* boundNode);
    void Accept(Visitor& visitor);
private:
    std::string irFilePath;
    std::string objectFilePath;
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ConversionTable conversionTable;
    Cm::Core::ClassConversionTable classConversionTable;
    Cm::Core::IrFunctionRepository irFunctionRepository;
    std::vector<std::unique_ptr<BoundNode>> boundNodes;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
