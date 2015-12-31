/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_NAME_MANGLING_INCLUDED
#define CM_SYM_NAME_MANGLING_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

std::string MakeAssemblyName(const std::string& name);
std::string MangleName(const std::string& namespaceName, const std::string& functionGroupName, const std::vector<TypeSymbol*>& templateArguments, const std::vector<ParameterSymbol*>& parameters);
std::string MakeGroupDocId(const std::string& functionGroupName);

void InitNameMangling();
void DoneNameMangling();

} } // namespace Cm::Sym

#endif // CM_SYM_NAME_MANGLING_INCLUDED
