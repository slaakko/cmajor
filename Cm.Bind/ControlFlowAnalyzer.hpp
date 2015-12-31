/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CONTROL_FLOW_ANALYZER_INCLUDED
#define CM_BIND_CONTROL_FLOW_ANALYZER_INCLUDED
#include <Cm.BoundTree/Visitor.hpp>

namespace Cm { namespace Bind {

class ControlFlowAnalyzer : public Cm::BoundTree::Visitor
{
public:
    ControlFlowAnalyzer();
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
};

} } // namespace Cm::Bind

#endif // CM_BIND_CONTROL_FLOW_ANALYZER_INCLUDED
