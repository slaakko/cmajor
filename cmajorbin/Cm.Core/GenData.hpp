/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_GEN_DATA_INCLUDED
#define CM_CORE_GEN_DATA_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Ir.Intf/Function.hpp>
#include <stack>
#include <unordered_set>

namespace Cm { namespace Core {

enum class GenFlags : uint8_t
{
    none = 0, 
    genJumpingBoolCode = 1 << 0,
    lvalue = 1 << 1,
    argByRef = 1 << 2,
    addrArg = 1 << 3,
    classTypeToPointerTypeConversion = 1 << 4,
    virtualCall = 1 << 5,
    labelSet = 1 << 6,
    memberVar = 1 << 7,
};

inline GenFlags operator&(GenFlags left, GenFlags right)
{
    return GenFlags(uint8_t(left) & uint8_t(right));
}

inline GenFlags operator|(GenFlags left, GenFlags right)
{
    return GenFlags(uint8_t(left) | uint8_t(right));
}

inline GenFlags operator~(GenFlags flag)
{
    return GenFlags(~uint8_t(flag));
}

inline bool GetFlag(GenFlags flag, GenFlags flags)
{
    return (flag & flags) != GenFlags::none;
}

inline void SetFlag(GenFlags flag, GenFlags& flags)
{
    flags = flags | flag;
}

inline void ResetFlag(GenFlags flag, GenFlags& flags)
{
    flags = flags & ~flag;
}

class GenData
{
public:
    GenData();
    GenData(const GenData&) = delete;
    GenData& operator=(const GenData&) = delete;
    GenData(GenData&& that) = delete;
    GenData& operator=(GenData&& that) = delete;
    bool IsEmpty() const { return objects.empty(); }
    Ir::Intf::Object* MainObject() const;
    void SetMainObject(Ir::Intf::Object* mainObject);
    void AddObject(Ir::Intf::Object* object) { objects.push_back(object); }
    Ir::Intf::Object* Arg1() const;
    Ir::Intf::Object* Arg2() const;
    std::vector<Ir::Intf::Object*> Args() const;
    const std::vector<Ir::Intf::Object*>& Objects() const { return objects; }
    std::vector<Ir::Intf::Object*>& Objects() { return objects; }
    void AddTrueTarget(Ir::Intf::LabelObject* trueTarget);
    void AddFalseTarget(Ir::Intf::LabelObject* falseTarget);
    void AddNextTarget(Ir::Intf::LabelObject* nextTarget);
    std::vector<Ir::Intf::LabelObject*>& NextTargets() { return nextTargets; }
    std::vector<Ir::Intf::LabelObject*>& TrueTargets() { return trueTargets; }
    std::vector<Ir::Intf::LabelObject*>& FalseTargets() { return falseTargets; }
    void MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets);
    void MergeData(std::shared_ptr<GenData> childData);
    Ir::Intf::LabelObject* GetLabel() const;
    void SetLabel(Ir::Intf::LabelObject* label_);
    void BackpatchTrueTargets(Ir::Intf::LabelObject* label_);
    void BackpatchFalseTargets(Ir::Intf::LabelObject* label_);
    void BackpatchNextTargets(Ir::Intf::LabelObject* label_);
private:
    Ir::Intf::LabelObject* label;
    std::vector<Ir::Intf::Object*> objects;
    std::vector<Ir::Intf::LabelObject*> nextTargets;
    std::vector<Ir::Intf::LabelObject*> trueTargets;
    std::vector<Ir::Intf::LabelObject*> falseTargets;
};

class CfgNode;

class ProfilingHandler
{
public:
    virtual void EmitEndProfiledFun(Ir::Intf::LabelObject* label) = 0;
};

class Emitter : public Ir::Intf::Emitter
{
public:
    Emitter();
	void SetIrFunction(Ir::Intf::Function* irFunction_) { irFunction = irFunction_; }
    Ir::Intf::Function* GetIrFunction() const { return irFunction; }
    void RequestLabelFor(std::shared_ptr<GenData> genData);
    void RemoveLabelRequestFor(std::shared_ptr<GenData> genData);
    void AddNextInstructionLabel(Ir::Intf::LabelObject* nextInstructionLabel) { nextInstructionLabels.insert(nextInstructionLabel); }
    void SetGotoTargetLabel(Ir::Intf::LabelObject* gotoTargetLabel_) { gotoTargetLabel = gotoTargetLabel_; }
    void Emit(Ir::Intf::Instruction* instruction) override;
    void Own(Ir::Intf::Object* object) override;
    void Own(Ir::Intf::Type* type) override;
    void Own(Ir::Intf::Function* fun) override;
    void UseCDebugNode(Ir::Intf::CDebugNode* cDebugNode_) { cDebugNode = cDebugNode_; }
    void SetActiveCfgNode(CfgNode* activeCfgNode_) { activeCfgNode = activeCfgNode_; }
    CfgNode* GetActiveCfgNode() const { return activeCfgNode; }
    void SetProfilingHandler(ProfilingHandler* profilingHandler_) { profilingHandler = profilingHandler_; }
    bool IrecPtrAllocated() const { return irecPtrAllocated; }
    void SetIrecPtrAllocated() { irecPtrAllocated = true; }
    bool TempBoolVarAllocted() const { return tempBoolVarAllocted; }
    void SetTempBoolVarAllocted() { tempBoolVarAllocted = true; }
    void SetInterfaceLookupFailed(Ir::Intf::Function* interfaceLookupFailed_) { interfaceLookupFailed = interfaceLookupFailed_; }
    Ir::Intf::Function* InterfaceLookupFailed() const { return interfaceLookupFailed; }
private:
    bool newLlvmSyntax;
    Ir::Intf::Function* irFunction;
    std::unordered_set<std::shared_ptr<GenData>> labelRequestSet;
    Ir::Intf::LabelObject* gotoTargetLabel;
    std::unordered_set<Ir::Intf::LabelObject*> nextInstructionLabels;
    std::string nextInstructionComment;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
    std::vector<std::unique_ptr<Ir::Intf::Type>> ownedTypes;
    std::vector<std::unique_ptr<Ir::Intf::Function>> ownedFuns;
    Ir::Intf::Function* interfaceLookupFailed;
    Ir::Intf::CDebugNode* cDebugNode;
    CfgNode* activeCfgNode;
    ProfilingHandler* profilingHandler;
    bool irecPtrAllocated;
    bool tempBoolVarAllocted;
};

class GenResult
{
public:
    GenResult();
    ~GenResult();
    GenResult(Emitter* emitter_, GenFlags flags_);
    GenResult(const GenResult&) = delete;
    GenResult& operator=(const GenResult&) = delete;
    GenResult(GenResult&& that) = delete;
    GenResult& operator=(GenResult&& that) = delete;
    GenFlags Flags() const { return flags; }
    void AddChild(std::shared_ptr<GenData> child);
    Ir::Intf::Object* MainObject() const { return genData->MainObject(); }
    void SetMainObject(Ir::Intf::Object* mainObject) { genData->SetMainObject(mainObject); }
    void SetMainObject(Cm::Sym::TypeSymbol* type, Cm::Sym::TypeRepository& typeRepository);
    void AddObject(Ir::Intf::Object* object) { genData->AddObject(object); }
    Ir::Intf::Object* Arg1() const { return genData->Arg1(); }
    Ir::Intf::Object* Arg2() const { return genData->Arg2(); }
    std::vector<Ir::Intf::Object*> Args() const { return genData->Args(); }
    const std::vector<Ir::Intf::Object*>& Objects() const { return genData->Objects(); }
    std::vector<Ir::Intf::Object*>& Objects() { return genData->Objects(); }
    void AddTrueTarget(Ir::Intf::LabelObject* trueTarget) { genData->AddTrueTarget(trueTarget); }
    void AddFalseTarget(Ir::Intf::LabelObject* falseTarget) { genData->AddFalseTarget(falseTarget); }
    void AddNextTarget(Ir::Intf::LabelObject* nextTarget) { genData->AddNextTarget(nextTarget);  }
    std::vector<Ir::Intf::LabelObject*>& NextTargets() { return genData->NextTargets(); }
    std::vector<Ir::Intf::LabelObject*>& TrueTargets() { return genData->TrueTargets(); }
    std::vector<Ir::Intf::LabelObject*>& FalseTargets() { return genData->FalseTargets(); }
    void MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets) { genData->MergeTargets(targets, fromTargets); }
    void BackpatchTrueTargets(Ir::Intf::LabelObject* label) { genData->BackpatchTrueTargets(label); }
    void BackpatchFalseTargets(Ir::Intf::LabelObject* label) { genData->BackpatchFalseTargets(label); }
    void BackpatchNextTargets(Ir::Intf::LabelObject* label) { genData->BackpatchNextTargets(label); }
    bool GenJumpingBoolCode() const { return GetFlag(GenFlags::genJumpingBoolCode, flags); }
    void SetGenJumpingBoolCode() { SetFlag(GenFlags::genJumpingBoolCode, flags); }
    void SetLvalue() { SetFlag(GenFlags::lvalue, flags); }
    bool ArgByRef() const { return GetFlag(GenFlags::argByRef, flags); }
    void SetArgByRef() { SetFlag(GenFlags::argByRef, flags); }
    bool AddrArg() const { return GetFlag(GenFlags::addrArg, flags); }
    void SetAddrArg() { SetFlag(GenFlags::addrArg, flags); }
    bool MemberVar() const { return GetFlag(GenFlags::memberVar, flags); }
    void SetMemberVar() { SetFlag(GenFlags::memberVar, flags); }
    bool ClassTypeToPointerTypeConversion() const { return GetFlag(GenFlags::classTypeToPointerTypeConversion, flags); }
    void SetClassTypeToPointerTypeConversion() { SetFlag(GenFlags::classTypeToPointerTypeConversion, flags); }
    bool GenerateVirtualCall() const { return GetFlag(GenFlags::virtualCall, flags); }
    void SetGenVirtualCall() { SetFlag(GenFlags::virtualCall, flags); }
    std::shared_ptr<GenData> GetChild(int index);
    void Merge(std::shared_ptr<GenResult> child);
    void Merge(std::shared_ptr<GenResult> child, bool insertFirst);
    Ir::Intf::LabelObject* GetLabel() const;
    void SetLabel(Ir::Intf::LabelObject* label);
    bool LabelSet() const { return GetFlag(GenFlags::labelSet, flags); }
    void SetLabelSet() { SetFlag(GenFlags::labelSet, flags); }
private:
    Emitter* emitter;
    GenFlags flags;
    std::shared_ptr<GenData> genData;
    std::vector<std::shared_ptr<GenData>> children;
};

class GenResultStack
{
public:
    void Push(std::shared_ptr<GenResult> genResult) { stack.push_back(genResult); }
    std::shared_ptr<GenResult> Pop() { std::shared_ptr<GenResult> top = stack.back(); stack.pop_back(); return top; }
    std::vector<std::shared_ptr<GenResult>>& Stack() { return stack; }
private:
    std::vector<std::shared_ptr<GenResult>> stack;
};

} } // namespace Cm::Core

#endif // CM_CORE_GEN_DATA_INCLUDED
