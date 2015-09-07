/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BUILD_MAIN_INCLUDED
#define CM_BUILD_MAIN_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Build {

bool GenerateMainCompileUnit(Cm::Sym::SymbolTable& symbolTable, const std::string& outputBasePath, const std::string& profile, std::vector<std::string>& objectFilePaths, bool changed);

} } // namespace Bm::Build

#endif // CM_BUILD_MAIN_INCLUDED
