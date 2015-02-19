/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_GEN_DATA_INCLUDED
#define CM_CORE_GEN_DATA_INCLUDED
#include <Cm.Core/Emitter.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <stack>
#include <unordered_set>

namespace Cm { namespace Core {

enum class GenFlags : uint8_t
{
    none = 0, 
    genJumpingBoolCode = 1 << 0,
    lvalue = 1 << 1,
    addrArg = 1 << 2,
    classTypeToPointerTypeConversion = 1 << 3,
    virtualCall = 1 << 4,
    labelSet = 1 << 5
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

class LabelHolder
{
public:
    LabelHolder(): label(nullptr) {}
    LabelHolder(LabelHolder&& that) : label(that.label) {}
    LabelHolder& operator=(LabelHolder&& that) { std::swap(label, that.label); }
    void SetLabel(Ir::Intf::LabelObject* label_) { label = label_; }
    Ir::Intf::LabelObject* GetLabel() const { return label; }
private:
    Ir::Intf::LabelObject* label;
};

class GenData
{
public:
    GenData();
    GenData(const GenData&) = delete;
    GenData& operator=(const GenData&) = delete;
    GenData(GenData&& that);
    GenData& operator=(GenData&& that);
    bool IsEmpty() const { return objects.empty(); }
    Ir::Intf::Object* MainObject() const;
    void SetMainObject(Ir::Intf::Object* mainObject);
    void AddObject(Ir::Intf::Object* object) { objects.push_back(object); }
    Ir::Intf::Object* Arg1() const;
    Ir::Intf::Object* Arg2() const;
    std::vector<Ir::Intf::Object*> Args() const;
    const std::vector<Ir::Intf::Object*>& Objects() const { return objects; }
    void AddTrueTarget(Ir::Intf::LabelObject* trueTarget);
    void AddFalseTarget(Ir::Intf::LabelObject* falseTarget);
    void AddNextTarget(Ir::Intf::LabelObject* nextTarget);
    void AddArgNextTarget(Ir::Intf::LabelObject* argNextTarget);
    std::vector<Ir::Intf::LabelObject*>& NextTargets() { return nextTargets; }
    std::vector<Ir::Intf::LabelObject*>& TrueTargets() { return trueTargets; }
    std::vector<Ir::Intf::LabelObject*>& FalseTargets() { return falseTargets; }
    std::vector<Ir::Intf::LabelObject*>& ArgNextTargets() { return argNextTargets; }
    void MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets);
    void MergeData(GenData& childData);
    Ir::Intf::LabelObject* GetLabel() const;
    void SetLabel(Ir::Intf::LabelObject* label);
    LabelHolder* GetLabelHolder() const { return labelHolder.get(); }
    void BackpatchTrueTargets(Ir::Intf::LabelObject* label);
    void BackpatchFalseTargets(Ir::Intf::LabelObject* label);
    void BackpatchNextTargets(Ir::Intf::LabelObject* label);
    void BackpatchArgNextTargets(Ir::Intf::LabelObject* label);
private:
    std::unique_ptr<LabelHolder> labelHolder;
    std::vector<Ir::Intf::Object*> objects;
    std::vector<Ir::Intf::LabelObject*> nextTargets;
    std::vector<Ir::Intf::LabelObject*> trueTargets;
    std::vector<Ir::Intf::LabelObject*> falseTargets;
    std::vector<Ir::Intf::LabelObject*> argNextTargets;
};

class Emitter : public Ir::Intf::Emitter
{
public:
    Emitter(Ir::Intf::Function* irFunction_);
    Ir::Intf::Function* GetIrFunction() const { return irFunction; }
    void RequestLabelFor(GenData& genData);
    void RemoveLabelRequestFor(GenData& genData);
    void AddNextInstructionLabel(Ir::Intf::LabelObject* nextInstructionLabel) { nextInstructionLabels.insert(nextInstructionLabel); }
    void Emit(Ir::Intf::Instruction* instruction) override;
    void Own(Ir::Intf::Object* object) override;
    void Own(Ir::Intf::Type* type) override;
    void Own(Ir::Intf::Function* fun) override;
private:
    Ir::Intf::Function* irFunction;
    std::unordered_set<LabelHolder*> labelRequestSet;
    Ir::Intf::LabelObject* gotoTargetLabel;
    std::unordered_set<Ir::Intf::LabelObject*> nextInstructionLabels;
    std::string nextInstructionComment;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
    std::vector<std::unique_ptr<Ir::Intf::Type>> ownedTypes;
    std::vector<std::unique_ptr<Ir::Intf::Function>> ownedFuns;
};

class GenResult
{
public:
    GenResult();
    ~GenResult();
    GenResult(Emitter* emitter_, GenFlags flags_);
    GenResult(const GenResult&) = delete;
    GenResult& operator=(const GenResult&) = delete;
    GenResult(GenResult&& that);
    GenResult& operator=(GenResult&& that);
    GenFlags Flags() const { return flags; }
    void AddChild(GenData&& child);
    Ir::Intf::Object* MainObject() const { return genData.MainObject(); }
    void SetMainObject(Ir::Intf::Object* mainObject) { genData.SetMainObject(mainObject); }
    void SetMainObject(Cm::Sym::TypeSymbol* type, Cm::Sym::TypeRepository& typeRepository);
    void AddObject(Ir::Intf::Object* object) { genData.AddObject(object); }
    Ir::Intf::Object* Arg1() const { return genData.Arg1(); }
    Ir::Intf::Object* Arg2() const { return genData.Arg2(); }
    std::vector<Ir::Intf::Object*> Args() const { return genData.Args(); }
    const std::vector<Ir::Intf::Object*>& Objects() const { return genData.Objects(); }
    void AddTrueTarget(Ir::Intf::LabelObject* trueTarget) { genData.AddTrueTarget(trueTarget); }
    void AddFalseTarget(Ir::Intf::LabelObject* falseTarget) { genData.AddFalseTarget(falseTarget); }
    void AddNextTarget(Ir::Intf::LabelObject* nextTarget) { genData.AddNextTarget(nextTarget);  }
    void AddArgNextTarget(Ir::Intf::LabelObject* argNextTarget) { genData.AddArgNextTarget(argNextTarget); }
    std::vector<Ir::Intf::LabelObject*>& NextTargets() { return genData.NextTargets(); }
    std::vector<Ir::Intf::LabelObject*>& TrueTargets() { return genData.TrueTargets(); }
    std::vector<Ir::Intf::LabelObject*>& FalseTargets() { return genData.FalseTargets(); }
    void MergeTargets(std::vector<Ir::Intf::LabelObject*>& targets, std::vector<Ir::Intf::LabelObject*>& fromTargets) { genData.MergeTargets(targets, fromTargets); }
    void BackpatchTrueTargets(Ir::Intf::LabelObject* label) { genData.BackpatchTrueTargets(label); }
    void BackpatchFalseTargets(Ir::Intf::LabelObject* label) { genData.BackpatchFalseTargets(label); }
    void BackpatchNextTargets(Ir::Intf::LabelObject* label) { genData.BackpatchNextTargets(label); }
    void BackpatchArgNextTargets(Ir::Intf::LabelObject* label) { genData.BackpatchArgNextTargets(label); }
    bool GenJumpingBoolCode() const { return GetFlag(GenFlags::genJumpingBoolCode, flags); }
    void SetGenJumpingBoolCode() { SetFlag(GenFlags::genJumpingBoolCode, flags); }
    void SetLvalue() { SetFlag(GenFlags::lvalue, flags); }
    bool AddrArg() const { return GetFlag(GenFlags::addrArg, flags); }
    void SetAddrArg() { SetFlag(GenFlags::addrArg, flags); }
    bool ClassTypeToPointerTypeConversion() const { return GetFlag(GenFlags::classTypeToPointerTypeConversion, flags); }
    void SetClassTypeToPointerTypeConversion() { SetFlag(GenFlags::classTypeToPointerTypeConversion, flags); }
    bool GenerateVirtualCall() const { return GetFlag(GenFlags::virtualCall, flags); }
    void SetGenVirtualCall() { SetFlag(GenFlags::virtualCall, flags); }
    GenData& GetChild(int index);
    void Merge(GenResult& child);
    void Merge(GenResult& child, bool insertFirst);
    Ir::Intf::LabelObject* GetLabel() const;
    void SetLabel(Ir::Intf::LabelObject* label);
    bool LabelSet() const { return GetFlag(GenFlags::labelSet, flags); }
    void SetLabelSet() { SetFlag(GenFlags::labelSet, flags); }
private:
    Emitter* emitter;
    GenFlags flags;
    GenData genData;
    std::vector<GenData> children;
};

class GenResultStack
{
public:
    void Push(GenResult&& genResult) { stack.push_back(std::move(genResult)); }
    GenResult Pop() { GenResult top = std::move(stack.back()); stack.pop_back(); return top; }
    std::vector<GenResult>& Stack() { return stack; }
private:
    std::vector<GenResult> stack;
};

} } // namespace Cm::Core

#endif // CM_CORE_GEN_DATA_INCLUDED
