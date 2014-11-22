/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef FACTORY_INCLUDED
#define FACTORY_INCLUDED

#include <Cm.Ast/Node.hpp>
#include <vector>

namespace Cm { namespace Ast {

class NodeCreator
{
public:
    virtual ~NodeCreator();
    virtual Node* CreateNode() = 0;
};

class Factory
{
public:
    Factory();
    static void Init();
    static Factory& Instance() 
    {
        return *instance;
    }
    void Register(NodeType nodeType, NodeCreator* creator);
    Node* CreateNode(NodeType nodeType);
private:
    static std::unique_ptr<Factory> instance;
    std::vector<std::unique_ptr<NodeCreator>> creators;
};

void InitFactory();

} } // namespace Cm::Ast

#endif // FACTORY_INCLUDED
