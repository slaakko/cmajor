/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Factory.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/DerivedTypeSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/DeclarationBlock.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/ConceptSymbol.hpp>

namespace Cm { namespace Sym {

ValueCreator::~ValueCreator()
{
}

template<typename T>
class ConcreteValueCreator : public ValueCreator
{
public:
    virtual Value* CreateValue()
    {
        return new T();
    }
};

std::unique_ptr<ValueFactory> ValueFactory::instance;

ValueFactory::ValueFactory() 
{
    creators.resize(int(ValueType::max));
}

void ValueFactory::Init()
{
    instance = std::unique_ptr<ValueFactory>(new ValueFactory());
}

void ValueFactory::Done()
{
    instance.reset();
}

ValueFactory& ValueFactory::Instance()
{
    return *instance;
}

void ValueFactory::Register(ValueType valueType, ValueCreator* creator)
{
    creators[int(valueType)] = std::unique_ptr<ValueCreator>(creator);
}

Value* ValueFactory::CreateValue(ValueType valueType)
{
    const std::unique_ptr<ValueCreator>& creator = creators[int(valueType)];
    if (creator)
    {
        Value* value = creators[int(valueType)]->CreateValue();
        return value;
    }
    else
    {
        throw std::runtime_error("no creator for value type '" + ValueTypeStr(valueType) + "'");
    }
}

SymbolCreator::~SymbolCreator()
{
}

template<typename T>
class ConcreteSymbolCreator : public SymbolCreator
{
public:
    virtual Symbol* CreateSymbol(const Span& span, const std::string& name)
    {
        return new T(span, name);
    }
    virtual Symbol* CreateBasicTypeSymbol()
    {
        return nullptr;
    };
};

template<typename T>
class ConcreteBasicTypeSymbolCreator : public SymbolCreator
{
public:
    virtual Symbol* CreateSymbol(const Span& span, const std::string& name)
    {
        return nullptr;
    }
    virtual Symbol* CreateBasicTypeSymbol()
    {
        return new T();
    }
};

std::unique_ptr<SymbolFactory> SymbolFactory::instance;

SymbolFactory::SymbolFactory()
{
    creators.resize(int(SymbolType::maxSymbol));
}

void SymbolFactory::Init()
{
    instance = std::unique_ptr<SymbolFactory>(new SymbolFactory());
}

void SymbolFactory::Done()
{
    instance.reset();
}

SymbolFactory& SymbolFactory::Instance()
{
    return *instance;
}

void SymbolFactory::Register(SymbolType symbolType, SymbolCreator* creator)
{
    creators[int(symbolType)] = std::unique_ptr<SymbolCreator>(creator);
}

Symbol* SymbolFactory::CreateBasicTypeSymbol(SymbolType basicTypeSymbolType)
{
    Symbol* value = creators[int(basicTypeSymbolType)]->CreateBasicTypeSymbol();
    if (value)
    {
        return value;
    }
    else
    {
        throw std::runtime_error("could not create basic type symbol");
    }
}

Symbol* SymbolFactory::CreateSymbol(SymbolType symbolType, const Span& span, const std::string& name)
{
    const std::unique_ptr<SymbolCreator>& creator = creators[int(symbolType)];
    if (creator)
    {
        Symbol* value = creator->CreateSymbol(span, name);
        if (value)
        {
            return value;
        }
        else
        {
            throw std::runtime_error("could not create symbol");
        }
    }
    else
    {
        throw std::runtime_error("no creator for symbol type '" + SymbolTypeStr(symbolType) + "'");
    }
}

void InitFactory()
{
    ValueFactory::Init();
    ValueFactory::Instance().Register(ValueType::boolValue, new ConcreteValueCreator<BoolValue>());
    ValueFactory::Instance().Register(ValueType::charValue, new ConcreteValueCreator<CharValue>());
    ValueFactory::Instance().Register(ValueType::sbyteValue, new ConcreteValueCreator<SByteValue>());
    ValueFactory::Instance().Register(ValueType::byteValue, new ConcreteValueCreator<ByteValue>());
    ValueFactory::Instance().Register(ValueType::shortValue, new ConcreteValueCreator<ShortValue>());
    ValueFactory::Instance().Register(ValueType::ushortValue, new ConcreteValueCreator<UShortValue>());
    ValueFactory::Instance().Register(ValueType::intValue, new ConcreteValueCreator<IntValue>());
    ValueFactory::Instance().Register(ValueType::uintValue, new ConcreteValueCreator<UIntValue>());
    ValueFactory::Instance().Register(ValueType::longValue, new ConcreteValueCreator<LongValue>());
    ValueFactory::Instance().Register(ValueType::ulongValue, new ConcreteValueCreator<ULongValue>());
    ValueFactory::Instance().Register(ValueType::floatValue, new ConcreteValueCreator<FloatValue>());
    ValueFactory::Instance().Register(ValueType::doubleValue, new ConcreteValueCreator<DoubleValue>());
    SymbolFactory::Init();
    SymbolFactory::Instance().Register(SymbolType::boolSymbol, new ConcreteBasicTypeSymbolCreator<BoolTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::charSymbol, new ConcreteBasicTypeSymbolCreator<CharTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::voidSymbol, new ConcreteBasicTypeSymbolCreator<VoidTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::sbyteSymbol, new ConcreteBasicTypeSymbolCreator<SByteTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::byteSymbol, new ConcreteBasicTypeSymbolCreator<ByteTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::shortSymbol, new ConcreteBasicTypeSymbolCreator<ShortTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::ushortSymbol, new ConcreteBasicTypeSymbolCreator<UShortTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::intSymbol, new ConcreteBasicTypeSymbolCreator<IntTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::uintSymbol, new ConcreteBasicTypeSymbolCreator<UIntTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::longSymbol, new ConcreteBasicTypeSymbolCreator<LongTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::ulongSymbol, new ConcreteBasicTypeSymbolCreator<ULongTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::floatSymbol, new ConcreteBasicTypeSymbolCreator<FloatTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::doubleSymbol, new ConcreteBasicTypeSymbolCreator<DoubleTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::nullptrSymbol, new ConcreteBasicTypeSymbolCreator<NullPtrTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::classSymbol, new ConcreteSymbolCreator<ClassTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::constantSymbol, new ConcreteSymbolCreator<ConstantSymbol>());
    SymbolFactory::Instance().Register(SymbolType::declarationBlock, new ConcreteSymbolCreator<DeclarationBlock>());
    SymbolFactory::Instance().Register(SymbolType::delegateSymbol, new ConcreteSymbolCreator<DelegateTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::classDelegateSymbol, new ConcreteSymbolCreator<ClassDelegateTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::enumTypeSymbol, new ConcreteSymbolCreator<EnumTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::enumConstantSymbol, new ConcreteSymbolCreator<EnumConstantSymbol>());
    SymbolFactory::Instance().Register(SymbolType::functionSymbol, new ConcreteSymbolCreator<FunctionSymbol>());
    SymbolFactory::Instance().Register(SymbolType::localVariableSymbol, new ConcreteSymbolCreator<LocalVariableSymbol>());
    SymbolFactory::Instance().Register(SymbolType::memberVariableSymbol, new ConcreteSymbolCreator<MemberVariableSymbol>());
    SymbolFactory::Instance().Register(SymbolType::namespaceSymbol, new ConcreteSymbolCreator<NamespaceSymbol>());
    SymbolFactory::Instance().Register(SymbolType::parameterSymbol, new ConcreteSymbolCreator<ParameterSymbol>());
    SymbolFactory::Instance().Register(SymbolType::typeParameterSymbol, new ConcreteSymbolCreator<TypeParameterSymbol>());
    SymbolFactory::Instance().Register(SymbolType::templateTypeSymbol, new ConcreteSymbolCreator<TemplateTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::derivedTypeSymbol, new ConcreteSymbolCreator<DerivedTypeSymbol>());
    SymbolFactory::Instance().Register(SymbolType::typedefSymbol, new ConcreteSymbolCreator<TypedefSymbol>());
    SymbolFactory::Instance().Register(SymbolType::conceptSymbol, new ConcreteSymbolCreator<ConceptSymbol>());
}

void DoneFactory()
{
    SymbolFactory::Done();
    ValueFactory::Done();
}

} } // namespace Cm::Sym
