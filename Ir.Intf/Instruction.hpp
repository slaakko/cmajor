/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_INSTRUCTION_INCLUDED
#define IR_INTF_INSTRUCTION_INCLUDED
#include <Ir.Intf/Label.hpp>
#include <Ir.Intf/Type.hpp>
#include <set>

namespace Ir { namespace Intf {

class MetadataNode;
class CDebugNode;

enum class Indirection
{
    none, deref, addr
};

enum class IConditionCode
{
    eq, ne, ugt, uge, ult, ule, sgt, sge, slt, sle
};

enum class FConditionCode
{
    false_, oeq, ogt, oge, olt, ole, one, ord, ueq, ugt, uge, ult, ule, une, uno, true_
};

class Instruction
{
public:
    Instruction(const std::string& name_);
    virtual ~Instruction();
    const std::string& Name() const { return name; }
    LabelObject* GetLabel() const { return label; }
    void SetLabel(LabelObject* label_) { label = label_; }
    void RemoveLabel() { label = nullptr; }
    virtual std::string ToString() const = 0;
    virtual void SetAlignment(int) {}
    virtual void SetInbounds() {}
    virtual void AddTargetLabels(std::set<std::string>& targetLabels) {}
    virtual bool Valid() const { return true; }
    virtual Object* GetResult() const { return nullptr; }
    virtual bool ReturnsResult() const { return false; }
    virtual bool IsTerminator() const { return false; }
    virtual bool IsUnconditionalBr() const { return false; }
    virtual bool IsRet() const { return false; }
    virtual bool IsCall() const { return false; }
    virtual LabelObject* GetTargetLabel() const { return nullptr; }
    void Remove() { removed = true; }
    bool Removed() const { return removed; }
    void SetComment(const std::string& comment_) { comment = comment_; }
    const std::string& Comment() const { return comment; }
    void SetSourcePosMetadataNode(MetadataNode* sourcePosMetadataNode_) { sourcePosMetadataNode = sourcePosMetadataNode_; }
    MetadataNode* GetSourcePosMetadataNode()  const { return sourcePosMetadataNode; }
    void SetCDebugNode(CDebugNode* cdebugNode_) { cdebugNode = cdebugNode_; }
    CDebugNode* GetCDebugNode() const { return cdebugNode; }
    void SetFunCallNode(CDebugNode* funCallNode_) { funCallNode = funCallNode_; }
    CDebugNode* GetFunCallNode() const { return funCallNode; }
private:
    bool removed;
    std::string name;
    LabelObject* label;
    std::string comment;
    MetadataNode* sourcePosMetadataNode;
    CDebugNode* cdebugNode;
    CDebugNode* funCallNode;
};

} } // namespace Ir::Intf

#endif // IR_INTF_INSTRUCTION_INCLUDED
