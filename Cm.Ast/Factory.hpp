/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_AST_FACTORY_INCLUDED
#define CM_AST_FACTORY_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <memory>
#include <vector>

namespace Cm { namespace Ast {

class NodeCreator
{
public:
    virtual ~NodeCreator();
    virtual Node* CreateNode(Span span) = 0;
};

class Factory
{
public:
    Factory();
    static void Init();
    static void Done();
    static Factory& Instance();
    void Register(NodeType nodeType, NodeCreator* creator);
    Node* CreateNode(NodeType nodeType, Span span);
private:
    static std::unique_ptr<Factory> instance;
    std::vector<std::unique_ptr<NodeCreator>> creators;
};

void InitFactory();
void DoneFactory();

} } // namespace Cm::Ast

#endif // CM_AST_FACTORY_INCLUDED
