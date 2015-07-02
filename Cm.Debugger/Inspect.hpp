/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_INSPECT_INCLUDED
#define CM_DEBUGGER_INSPECT_INCLUDED
#include <Cm.Debugger/InspectNode.hpp>
#include <Cm.Debugger/Type.hpp>
#include <Cm.Debugger/Value.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <stack>

namespace Cm { namespace Debugger {

class Gdb;
class DebugInfo;

class PrintExpr
{
public:
    PrintExpr(const std::string& text_, TypeExpr* typeExpr_);
    const std::string& Text() const { return text; }
    std::string& Text() { return text; }
    TypeExpr* GetTypeExpr() const { return typeExpr.get(); }
    void SetTypeExpr(TypeExpr* typeExpr_);
    TypeExpr* GetDisplayTypeExpr() const { return displayTypeExpr.get(); }
    void SetDisplayTypeExpr(TypeExpr* displayTypeExpr_);
private:
    std::string text;
    std::unique_ptr<TypeExpr> typeExpr;
    std::unique_ptr<TypeExpr> displayTypeExpr;
};

class Inspector : public InspectNodeVisitor
{
public:
    Inspector(Gdb& gdb_, DebugInfo& debugInfo_, Cm::Core::CfgNode* currentNode_);
    void Parse(const std::string& expr_);
    std::vector<std::unique_ptr<Result>> GetResults();
private:
    std::unique_ptr<InspectNode> inspectNode;
    std::string expr;
    Gdb& gdb;
    DebugInfo& debugInfo;
    Cm::Core::CfgNode* currentNode;
    std::stack<PrintExpr> printExprStack;
    std::vector<std::unique_ptr<Result>> results;
    void PushPrintExpr(PrintExpr&& printExpr);
    PrintExpr PopPrintExpr();
    void Visit(SingleNode& singleNode) override;
    void Visit(ContentNode& contentNode) override;
    void Visit(LocalNode& localNode) override;
    void Visit(HandleNode& handleNode) override;
    void Visit(DerefNode& derefNode) override;
    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void Visit(ParenthesesNode& parenthesesNode) override;
    void InspectClass(const PrintExpr& printExpr, TypeExpr* typeExpr);
    void InspectPointer(const PrintExpr& printExpr, TypeExpr* typeExpr);
    void InspectVirtualClass(Cm::Core::ClassDebugInfo* classDebugInfo, const PrintExpr& printExpr, const PrintExpr& derefExpr, TypeExpr* typeExpr);
    std::string GetActualClassName(Cm::Core::ClassDebugInfo* classDebugInfo, const PrintExpr& printExpr);
    void InspectList(const PrintExpr& printExpr);
    void InspectForwardList(const PrintExpr& printExpr);
    void InspectLinkedList(const PrintExpr& printExpr);
    void InspectSet(const PrintExpr& printExpr);
    void InspectMap(const PrintExpr& printExpr);
    void InspectTree(const PrintExpr& printExpr);
    void InspectHashSet(const PrintExpr& printExpr);
    void InspectHashMap(const PrintExpr& printExpr);
    void InspectHashtable(const PrintExpr& printExpr);
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_INSPECT_INCLUDED
