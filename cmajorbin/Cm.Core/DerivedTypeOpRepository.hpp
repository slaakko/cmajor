/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_DERIVED_TYPE_OP_REPOSITORY_INCLUDED
#define CM_CORE_DERIVED_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/ConversionTable.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Core {

using Cm::Parsing::Span;

class DerivedTypeOpCache
{
public:
    DerivedTypeOpCache();
    Cm::Sym::FunctionSymbol* GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetPtrToVoidPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable, const Span& span);
    Cm::Sym::FunctionSymbol* GetVoidPtrToPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, const Span& span);
    Cm::Sym::FunctionSymbol* GetNullPtrToPtrConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable);
    Cm::Sym::FunctionSymbol* GetVoidPtrToUlongConversion(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type, Cm::Sym::ConversionTable& conversionTable, const Span& span);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpAddPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpAddIntPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpSubPtrInt(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpSubPtrPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpDeref(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpIncPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpDecPtr(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpAddrOf(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetOpArrow(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> ptrToVoidPtrConversion;
    std::unique_ptr<Cm::Sym::FunctionSymbol> voidPtrToPtrConversion;
    std::unique_ptr<Cm::Sym::FunctionSymbol> nullPtrToPtrConversion;
    std::unique_ptr<Cm::Sym::FunctionSymbol> voidPtrToULongConversion;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opEqual;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opLess;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opAddPtrInt;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opAddIntPtr;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opSubPtrInt;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opSubPtrPtr;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opDeref;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opIncPtr;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opDecPtr;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opAddrOf;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opArrow;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, DerivedTypeOpCache>  DerivedTypeOpCacheMap;

class DerivedTypeOpGroup
{
public:
    virtual ~DerivedTypeOpGroup();
    virtual void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span, 
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class ConstructorOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span, 
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class AssignmentOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class EqualityOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class LessOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class AdditiveOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class SubtractiveOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DerefOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class IncrementOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DecrementOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class AddressOfOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ArrowOpGroup : public DerivedTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, DerivedTypeOpCacheMap& derivedTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DerivedTypeOpRepository
{
public:
    DerivedTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_);
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span, 
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions);
    void InsertPointerConversionsToConversionTable(Cm::Sym::ConversionTable& conversionTable, Cm::Sym::TypeSymbol* pointerType, const Cm::Parsing::Span& span);
private:
    Cm::Sym::TypeRepository& typeRepository;
    DerivedTypeOpCacheMap derivedTypeOpCacheMap;
    typedef std::unordered_map<std::string, DerivedTypeOpGroup*> DerivedTypeOpGroupMap;
    typedef DerivedTypeOpGroupMap::const_iterator DerivedTypeOpGroupMapIt;
    DerivedTypeOpGroupMap derivedTypeOpGroupMap;
    ConstructorOpGroup constructorOpGroup;
    AssignmentOpGroup assignmentOpGroup;
    EqualityOpGroup equalityOpGroup;
    LessOpGroup lessOpGroup;
    AdditiveOpGroup additiveOpGroup;
    SubtractiveOpGroup subtractiveOpGroup;
    DerefOpGroup derefOpGroup;
    IncrementOpGroup incrementOpGroup;
    DecrementOpGroup decrementOpGroup;
    AddressOfOpGroup addressOfOpGroup;
    ArrowOpGroup arrowOpGroup;
};

} } // namespace Cm::Core

#endif // CM_CORE_DERIVED_TYPE_OP_REPOSITORY_INCLUDED
