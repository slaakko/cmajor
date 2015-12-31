/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_SYNTAX_LIBRARY_INCLUDED
#define CM_PARSING_SYNTAX_LIBRARY_INCLUDED

#include <Cm.Parsing.Syntax/Project.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

void GenerateLibraryFile(Project* project, Cm::Parsing::ParsingDomain* parsingDomain);

} } } // namespace Cm::Parsing::Syntax

#endif // CM_PARSING_SYNTAX_LIBRARY_INCLUDED
