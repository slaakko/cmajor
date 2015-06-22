/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_INSPECT_NODE_INCLUDED
#define CM_DEBUGGER_INSPECT_NODE_INCLUDED
#include <memory>
#include <string>

namespace Cm { namespace Debugger {

class InspectNodeVisitor;

class InspectNode
{
public:
    virtual ~InspectNode();
    virtual void Accept(InspectNodeVisitor& visitor) = 0;
};

class SingleNode : public InspectNode
{
public:
    SingleNode(InspectNode* subject_);
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
};

class ContentNode : public InspectNode
{
public:
    ContentNode(InspectNode* subject_);
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
};

class DerefNode : public InspectNode
{
public:
    DerefNode(InspectNode* subject_);
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
};

class DotNode : public InspectNode
{
public:
    DotNode(InspectNode* subject_, const std::string& memberId_);
    const std::string& MemberId() const { return memberId; }
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
    std::string memberId;
};

class ArrowNode : public InspectNode
{
public:
    ArrowNode(InspectNode* subject_, const std::string& memberId_);
    const std::string& MemberId() const { return memberId; }
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
    std::string memberId;
};

class LocalNode : public InspectNode
{
public:
    LocalNode(const std::string& localVarName_);
    const std::string& LocalVarName() const { return localVarName; }
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::string localVarName;
};

class HandleNode : public InspectNode
{
public:
    HandleNode(int handle_);
    int Handle() const { return handle; }
    void Accept(InspectNodeVisitor& visitor) override;
private:
    int handle;
};

class ParenthesesNode : public InspectNode
{
public:
    ParenthesesNode(InspectNode* subject_);
    void Accept(InspectNodeVisitor& visitor) override;
private:
    std::unique_ptr<InspectNode> subject;
};

class InspectNodeVisitor
{
public:
    virtual ~InspectNodeVisitor();
    virtual void Visit(SingleNode& singleNode) {}
    virtual void Visit(ContentNode& contentNode) {}
    virtual void Visit(LocalNode& localNode) {}
    virtual void Visit(HandleNode& handleNode) {}
    virtual void Visit(DerefNode& derefNode) {}
    virtual void Visit(DotNode& dotNode) {}
    virtual void Visit(ArrowNode& arrowNode) {}
    virtual void Visit(ParenthesesNode& parenthesesNode) {}
};

} } // namespace Cm::Debugger

#endif // CM_DEBUGGER_INSPECT_NODE_INCLUDED
