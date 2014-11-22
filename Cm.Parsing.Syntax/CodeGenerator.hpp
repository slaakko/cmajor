/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_SYNTAX_CODEGENERATOR_INCLUDED
#define CM_PARSING_SYNTAX_CODEGENERATOR_INCLUDED

#include <Cm.Parsing.Syntax/ParserFileContent.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

void SetForceCodeGen(bool force);
void GenerateCode(const std::vector<std::unique_ptr<ParserFileContent>>& parserFiles);

} } } // namespace Cm::Parsing::Syntax

#endif // CM_PARSING_SYNTAX_CODEGENERATOR_INCLUDED
