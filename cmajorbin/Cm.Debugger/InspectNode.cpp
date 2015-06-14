/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/InspectNode.hpp>

namespace Cm { namespace Debugger {

InspectNode::~InspectNode()
{
}

SingleNode::SingleNode(InspectNode* subject_) : subject(subject_)
{
}

void SingleNode::Accept(InspectNodeVisitor& visitor)
{
    subject->Accept(visitor);
    visitor.Visit(*this);
}

ContentNode::ContentNode(InspectNode* subject_) : subject(subject_)
{
}

void ContentNode::Accept(InspectNodeVisitor& visitor)
{
    subject->Accept(visitor);
    visitor.Visit(*this);
}

DerefNode::DerefNode(InspectNode* subject_) : subject(subject_)
{
}

void DerefNode::Accept(InspectNodeVisitor& visitor)
{
    subject->Accept(visitor);
    visitor.Visit(*this);
}

DotNode::DotNode(InspectNode* subject_, const std::string& memberId_) : subject(subject_), memberId(memberId_)
{
}

void DotNode::Accept(InspectNodeVisitor& visitor)
{
    subject->Accept(visitor);
    visitor.Visit(*this);
}

ArrowNode::ArrowNode(InspectNode* subject_, const std::string& memberId_) : subject(subject_), memberId(memberId_)
{
}

void ArrowNode::Accept(InspectNodeVisitor& visitor)
{
    subject->Accept(visitor);
    visitor.Visit(*this);
}

LocalNode::LocalNode(const std::string& localVarName_) : localVarName(localVarName_)
{
}

void LocalNode::Accept(InspectNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

HandleNode::HandleNode(int handle_, const std::string& typeExpr_) : handle(handle_), typeExpr(typeExpr_)
{
}

void HandleNode::Accept(InspectNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

ParenthesesNode::ParenthesesNode(InspectNode* subject_) : subject(subject_)
{
}

void ParenthesesNode::Accept(InspectNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

InspectNodeVisitor::~InspectNodeVisitor()
{
}

} } // namespace Cm::Debugger
