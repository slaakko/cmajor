/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BUILD_BUILD_INCLUDED
#define CM_BUILD_BUILD_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>

namespace Cm { namespace Build {

void BuildSolution(const std::string& solutionFilePath);
void BuildProject(const std::string& projectFilePath);
void Emit(Cm::Sym::TypeRepository& typeRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
void GenerateObjectCode(Cm::BoundTree::BoundCompileUnit& boundCompileUnit);

} } // namespace Bm::Build

#endif // CM_BUILD_BUILD_INCLUDED
