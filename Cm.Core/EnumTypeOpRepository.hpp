/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_ENUM_TYPE_OP_REPOSITORY_INCLUDED
#define CM_ENUM_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/ConversionTable.hpp>

namespace Cm { namespace Core {

class EnumTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
    Cm::Sym::FunctionSymbol* GetConversionToUnderlyingType(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType, Cm::Sym::ConversionTable& conversionTable);
    Cm::Sym::FunctionSymbol* GetConversionFromUnderlyingType(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* enumType);
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opEqual;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opLess;
    std::unique_ptr<Cm::Sym::FunctionSymbol> conversionToUnderlyingType;
    std::unique_ptr<Cm::Sym::FunctionSymbol> conversionFromUnderlyingType;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, EnumTypeOpCache>  EnumTypeOpCacheMap;

class EnumTypeOpGroup
{
public:
    virtual ~EnumTypeOpGroup();
    virtual void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class EnumConstructorOpGroup : public EnumTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class EnumAssignmentOpGroup : public EnumTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class EnumEqualOpGroup : public EnumTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class EnumLessOpGroup : public EnumTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        Cm::Sym::TypeRepository& typeRepository, EnumTypeOpCacheMap& enumTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class EnumTypeOpRepository
{
public:
    EnumTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_);
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable, const Cm::Parsing::Span& span,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions);
    void InsertEnumConversionsToConversionTable(Cm::Sym::ConversionTable& conversionTable, Cm::Sym::TypeSymbol* enumType, const Cm::Parsing::Span& span);
private:
    Cm::Sym::TypeRepository& typeRepository;
    typedef std::unordered_map<std::string, EnumTypeOpGroup*> EnumTypeOpGroupMap;
    typedef EnumTypeOpGroupMap::const_iterator EnumTypeOpGroupMapIt;
    EnumTypeOpCacheMap enumTypeOpCacheMap;
    EnumTypeOpGroupMap enumTypeOpGroupMap;
    EnumConstructorOpGroup enumConstructorOpGroup;
    EnumAssignmentOpGroup enumAssignmentOpGroup;
    EnumEqualOpGroup enumEqualOpGroup;
    EnumLessOpGroup enumLessOpGroup;
};

} } // namespace Cm::Core

#endif // CM_ENUM_TYPE_OP_REPOSITORY_INCLUDED
