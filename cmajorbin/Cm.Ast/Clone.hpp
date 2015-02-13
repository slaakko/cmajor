/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_CLONE_INCLUDED
#define CM_AST_CLONE_INCLUDED

namespace Cm { namespace Ast {

class CloneContext
{
public:
    CloneContext();
    void SetInstantiateClassNode() { instantiateClassNode = true; }
    bool InstantiateClassNode() const { return instantiateClassNode; }
private:
    bool instantiateClassNode;
};

} } // namespace Cm::Ast

#endif // CM_AST_CLONE_INCLUDED
