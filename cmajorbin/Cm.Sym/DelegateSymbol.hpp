/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DELEGATE_SYMBOL_INCLUDED
#define CM_SYM_DELEGATE_SYMBOL_INCLUDED
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Delegate.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Sym {

enum class DelegateTypeSymbolFlags : uint8_t
{
    none = 0,
    nothrow = 1 << 0,
    throw_ = 1 << 1
};

inline DelegateTypeSymbolFlags operator&(DelegateTypeSymbolFlags left, DelegateTypeSymbolFlags right)
{
    return DelegateTypeSymbolFlags(uint8_t(left) & uint8_t(right));
}

inline DelegateTypeSymbolFlags operator|(DelegateTypeSymbolFlags left, DelegateTypeSymbolFlags right)
{
    return DelegateTypeSymbolFlags(uint8_t(left) | uint8_t(right));
}

class DelegateTypeSymbol : public TypeSymbol
{
public:
    DelegateTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::delegateSymbol; }
    std::string TypeString() const override { return "delegate"; };
    bool IsDelegateTypeSymbol() const override { return true; }
    std::string GetMangleId() const override;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void AddSymbol(Symbol* symbol) override;
    void SetType(TypeSymbol* type_, int index) override;
    void SetReturnType(TypeSymbol* returnType_);
    TypeSymbol* GetReturnType() const { return returnType; }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    bool IsExportSymbol() const override;
    bool IsNothrow() const
    {
        return GetFlag(DelegateTypeSymbolFlags::nothrow);
    }
    void SetNothrow()
    {
        SetFlag(DelegateTypeSymbolFlags::nothrow);
    }
    bool IsThrow() const
    {
        return GetFlag(DelegateTypeSymbolFlags::throw_);
    }
    void SetThrow()
    {
        SetFlag(DelegateTypeSymbolFlags::throw_);
    }
    void MakeIrType() override;
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes) override;
    bool IsValueTypeSymbol() const override { return true; }
    void Dump(CodeFormatter& formatter) override;
    std::string Syntax() const override;
    void ReplaceReplicaTypes() override;
private:
    DelegateTypeSymbolFlags flags;
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    bool GetFlag(DelegateTypeSymbolFlags flag) const
    {
        return (flags & flag) != DelegateTypeSymbolFlags::none;
    }
    void SetFlag(DelegateTypeSymbolFlags flag)
    {
        flags = flags | flag;
    }
};

enum class ClassDelegateTypeSymbolFlags : uint8_t
{
    none = 0,
    nothrow = 1 << 0,
    throw_ = 1 << 1
};

inline ClassDelegateTypeSymbolFlags operator&(ClassDelegateTypeSymbolFlags left, ClassDelegateTypeSymbolFlags right)
{
    return ClassDelegateTypeSymbolFlags(uint8_t(left) & uint8_t(right));
}

inline ClassDelegateTypeSymbolFlags operator|(ClassDelegateTypeSymbolFlags left, ClassDelegateTypeSymbolFlags right)
{
    return ClassDelegateTypeSymbolFlags(uint8_t(left) | uint8_t(right));
}

class ClassDelegateTypeSymbol : public ClassTypeSymbol
{
public:
    ClassDelegateTypeSymbol(const Span& span_, const std::string& name_);
    ClassDelegateTypeSymbol(const Span& span_, const std::string& name_, bool getNextId, uint64_t cid_);
    SymbolType GetSymbolType() const override { return SymbolType::classDelegateSymbol; }
    std::string TypeString() const override { return "class delegate"; };
    std::string GetMangleId() const override;
    bool IsExportSymbol() const override;
    bool IsClassDelegateTypeSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void AddSymbol(Symbol* symbol) override;
    void SetType(TypeSymbol* type_, int index) override;
    void SetReturnType(TypeSymbol* returnType_);
    TypeSymbol* GetReturnType() const { return returnType; }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    bool IsNothrow() const
    {
        return GetFlag(ClassDelegateTypeSymbolFlags::nothrow);
    }
    void SetNothrow()
    {
        SetFlag(ClassDelegateTypeSymbolFlags::nothrow);
    }
    bool IsThrow() const
    {
        return GetFlag(ClassDelegateTypeSymbolFlags::throw_);
    }
    void SetThrow()
    {
        SetFlag(ClassDelegateTypeSymbolFlags::throw_);
    }
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes) override;
    void Dump(CodeFormatter& formatter) override;
    std::string Syntax() const override;
    void ReplaceReplicaTypes() override;
private:
    ClassDelegateTypeSymbolFlags flags;
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    bool GetFlag(ClassDelegateTypeSymbolFlags flag) const
    {
        return (flags & flag) != ClassDelegateTypeSymbolFlags::none;
    }
    void SetFlag(ClassDelegateTypeSymbolFlags flag)
    {
        flags = flags | flag;
    }
};

} } // namespace Cm::Sym

#endif // CM_SYM_DELEGATE_SYMBOL_INCLUDED
