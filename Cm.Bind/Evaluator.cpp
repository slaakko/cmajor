/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Bind/OverloadResolution.hpp>
#include <Cm.Bind/Function.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/SymbolTypeSet.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/DeclarationBlock.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Expression.hpp>
#include <functional>
#include <utility>

namespace Cm { namespace Bind {

class VariableValueSymbol : public Cm::Sym::VariableSymbol
{
public:
    VariableValueSymbol(const Cm::Ast::Span& span_, const std::string& name_);
    VariableValueSymbol(const Cm::Ast::Span& span_, const std::string& name_, Cm::Sym::Value* value_);
    Cm::Sym::SymbolType GetSymbolType() const override { return Cm::Sym::SymbolType::variableValueSymbol; }
    bool IsVariableValueSymbol() const override { return true; }
    Cm::Sym::Value* GetValue() const { return value.get(); }
    void SetValue(Cm::Sym::Value* value_);
private:
    std::unique_ptr<Cm::Sym::Value> value;
};

VariableValueSymbol::VariableValueSymbol(const Cm::Ast::Span& span_, const std::string& name_) : Cm::Sym::VariableSymbol(span_, name_)
{
}

VariableValueSymbol::VariableValueSymbol(const Cm::Ast::Span& span_, const std::string& name_, Cm::Sym::Value* value_) : Cm::Sym::VariableSymbol(span_, name_), value(value_)
{
}

void VariableValueSymbol::SetValue(Cm::Sym::Value* value_)
{
    value.reset(value_);
}

using Cm::Parsing::Span;

typedef Cm::Sym::Value* (*BinaryOperatorFun)(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span);
typedef Cm::Sym::Value* (*UnaryOperatorFun)(Cm::Sym::Value* subject, const Span& span);

Cm::Sym::Value* NotSupported(Cm::Sym::Value* subject, const Span& span)
{
    throw Cm::Core::Exception("operation not supported for type " + ValueTypeStr(subject->GetValueType()), span);
}

Cm::Sym::Value* NotSupported(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    throw Cm::Core::Exception("operation not supported for types " + ValueTypeStr(left->GetValueType()) + " and " + ValueTypeStr(right->GetValueType()), span);
}

template<typename ValueT, typename Op>
Cm::Sym::Value* UnaryEvaluate(Cm::Sym::Value* subject, Op op)
{
    ValueT* subject_ = static_cast<ValueT*>(subject);
    return new ValueT(op(subject_->Value()));
}

template<typename ValueT>
Cm::Sym::Value* Not(Cm::Sym::Value* subject, const Span& span)
{
    return UnaryEvaluate<ValueT>(subject, std::logical_not<typename ValueT::OperandType>());
}

UnaryOperatorFun not_[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Not<Cm::Sym::BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template <typename T>
struct identity
{
    T operator()(const T& x) const { return x; }
};

template<typename ValueT>
Cm::Sym::Value* UnaryPlus(Cm::Sym::Value* subject, const Span& span)
{
    return UnaryEvaluate<ValueT>(subject, identity<typename ValueT::OperandType>());
}

UnaryOperatorFun unaryPlus[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    UnaryPlus<Cm::Sym::SByteValue>, UnaryPlus<Cm::Sym::ByteValue>, UnaryPlus<Cm::Sym::ShortValue>, UnaryPlus<Cm::Sym::UShortValue>, UnaryPlus<Cm::Sym::IntValue>, UnaryPlus<Cm::Sym::UIntValue>, 
    UnaryPlus<Cm::Sym::LongValue>, UnaryPlus<Cm::Sym::ULongValue>, UnaryPlus<Cm::Sym::FloatValue>, UnaryPlus<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Negate(Cm::Sym::Value* subject, const Span& span)
{
    return UnaryEvaluate<ValueT>(subject, std::negate<typename ValueT::OperandType>());
}

UnaryOperatorFun negate[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Negate<Cm::Sym::SByteValue>, Negate<Cm::Sym::ByteValue>, Negate<Cm::Sym::ShortValue>, Negate<Cm::Sym::UShortValue>, Negate<Cm::Sym::IntValue>, Negate<Cm::Sym::UIntValue>, 
    Negate<Cm::Sym::LongValue>, Negate<Cm::Sym::ULongValue>, Negate<Cm::Sym::FloatValue>, Negate<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template <typename T>
struct bit_not
{
    typedef T result_type;
    typedef T argument_type;
    T operator()(const T& x) const
    {
        return ~x;
    }
};

template<typename ValueT>
Cm::Sym::Value* Complement(Cm::Sym::Value* subject, const Span& span)
{
    return UnaryEvaluate<ValueT>(subject, bit_not<typename ValueT::OperandType>());
}

UnaryOperatorFun complement[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Complement<Cm::Sym::SByteValue>, Complement<Cm::Sym::ByteValue>, Complement<Cm::Sym::ShortValue>, Complement<Cm::Sym::UShortValue>, Complement<Cm::Sym::IntValue>, Complement<Cm::Sym::UIntValue>, 
    Complement<Cm::Sym::LongValue>, Complement<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

void EvaluateUnaryOp(Cm::Sym::ValueType targetType, Cm::Sym::EvaluationStack& stack, UnaryOperatorFun* fun, bool cast, const Span& span)
{
    std::unique_ptr<Cm::Sym::Value> subject(stack.Pop());
    Cm::Sym::ValueType subjectType = subject->GetValueType();
    Cm::Sym::ValueType operationType = subjectType;
    if (targetType > subjectType)
    {
        operationType = targetType;
    }
    std::unique_ptr<Cm::Sym::Value> subject_(subject->As(operationType, cast, span));
    UnaryOperatorFun operation = fun[uint8_t(operationType)];
    stack.Push(operation(subject_.get(), span));
}

template <typename ValueT, typename Op>
Cm::Sym::Value* BinaryEvaluate(Cm::Sym::Value* left, Cm::Sym::Value* right, Op op)
{
    ValueT* left_ = static_cast<ValueT*>(left);
    ValueT* right_ = static_cast<ValueT*>(right);
    return new ValueT(op(left_->Value(), right_->Value()));
}

template<typename T>
struct shiftLeftFun : std::binary_function<T, T, T>
{
    T operator()(const T& left, const T& right) const
    {
        return left << right;
    }
};

template<typename ValueT>
Cm::Sym::Value* ShiftLeft(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, shiftLeftFun<typename ValueT::OperandType>());
}

BinaryOperatorFun shiftLeft[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    ShiftLeft<Cm::Sym::SByteValue>, ShiftLeft<Cm::Sym::ByteValue>, ShiftLeft<Cm::Sym::ShortValue>, ShiftLeft<Cm::Sym::UShortValue>, ShiftLeft<Cm::Sym::IntValue>, ShiftLeft<Cm::Sym::UIntValue>, 
    ShiftLeft<Cm::Sym::LongValue>, ShiftLeft<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename T>
struct shiftRightFun : std::binary_function<T, T, T>
{
    T operator()(const T& left, const T& right) const
    {
        return left >> right;
    }
};

template<typename ValueT>
Cm::Sym::Value* ShiftRight(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, shiftRightFun<typename ValueT::OperandType>());
}

BinaryOperatorFun shiftRight[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    ShiftRight<Cm::Sym::SByteValue>, ShiftRight<Cm::Sym::ByteValue>, ShiftRight<Cm::Sym::ShortValue>, ShiftRight<Cm::Sym::UShortValue>, ShiftRight<Cm::Sym::IntValue>, ShiftRight<Cm::Sym::UIntValue>, 
    ShiftRight<Cm::Sym::LongValue>, ShiftRight<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Add(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::plus<typename ValueT::OperandType>());
}

BinaryOperatorFun add[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Add<Cm::Sym::SByteValue>, Add<Cm::Sym::ByteValue>, Add<Cm::Sym::ShortValue>, Add<Cm::Sym::UShortValue>, Add<Cm::Sym::IntValue>, Add<Cm::Sym::UIntValue>, 
    Add<Cm::Sym::LongValue>, Add<Cm::Sym::ULongValue>, Add<Cm::Sym::FloatValue>, Add<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Sub(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::minus<typename ValueT::OperandType>());
}

BinaryOperatorFun sub[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Sub<Cm::Sym::SByteValue>, Sub<Cm::Sym::ByteValue>, Sub<Cm::Sym::ShortValue>, Sub<Cm::Sym::UShortValue>, Sub<Cm::Sym::IntValue>, Sub<Cm::Sym::UIntValue>, 
    Sub<Cm::Sym::LongValue>, Sub<Cm::Sym::ULongValue>, Sub<Cm::Sym::FloatValue>, Sub<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Mul(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::multiplies<typename ValueT::OperandType>());
}

BinaryOperatorFun mul[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Mul<Cm::Sym::SByteValue>, Mul<Cm::Sym::ByteValue>, Mul<Cm::Sym::ShortValue>, Mul<Cm::Sym::UShortValue>, Mul<Cm::Sym::IntValue>, Mul<Cm::Sym::UIntValue>, 
    Mul<Cm::Sym::LongValue>, Mul<Cm::Sym::ULongValue>, Mul<Cm::Sym::FloatValue>, Mul<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Div(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::divides<typename ValueT::OperandType>());
}

BinaryOperatorFun div[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Div<Cm::Sym::SByteValue>, Div<Cm::Sym::ByteValue>, Div<Cm::Sym::ShortValue>, Div<Cm::Sym::UShortValue>, Div<Cm::Sym::IntValue>, Div<Cm::Sym::UIntValue>, 
    Div<Cm::Sym::LongValue>, Div<Cm::Sym::ULongValue>, Div<Cm::Sym::FloatValue>, Div<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Rem(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::modulus<typename ValueT::OperandType>());
}

BinaryOperatorFun rem[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    Rem<Cm::Sym::SByteValue>, Rem<Cm::Sym::ByteValue>, Rem<Cm::Sym::ShortValue>, Rem<Cm::Sym::UShortValue>, Rem<Cm::Sym::IntValue>, Rem<Cm::Sym::UIntValue>, 
    Rem<Cm::Sym::LongValue>, Rem<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitAnd(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_and<typename ValueT::OperandType>());
}

BinaryOperatorFun bitAnd[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    BitAnd<Cm::Sym::SByteValue>, BitAnd<Cm::Sym::ByteValue>, BitAnd<Cm::Sym::ShortValue>, BitAnd<Cm::Sym::UShortValue>, BitAnd<Cm::Sym::IntValue>, BitAnd<Cm::Sym::UIntValue>, 
    BitAnd<Cm::Sym::LongValue>, BitAnd<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitOr(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_or<typename ValueT::OperandType>());
}

BinaryOperatorFun bitOr[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    BitOr<Cm::Sym::SByteValue>, BitOr<Cm::Sym::ByteValue>, BitOr<Cm::Sym::ShortValue>, BitOr<Cm::Sym::UShortValue>, BitOr<Cm::Sym::IntValue>, BitOr<Cm::Sym::UIntValue>, 
    BitOr<Cm::Sym::LongValue>, BitOr<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitXor(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_xor<typename ValueT::OperandType>());
}

BinaryOperatorFun bitXor[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported,
    BitXor<Cm::Sym::SByteValue>, BitXor<Cm::Sym::ByteValue>, BitXor<Cm::Sym::ShortValue>, BitXor<Cm::Sym::UShortValue>, BitXor<Cm::Sym::IntValue>, BitXor<Cm::Sym::UIntValue>, 
    BitXor<Cm::Sym::LongValue>, BitXor<Cm::Sym::ULongValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Disjunction(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_or<typename ValueT::OperandType>());
}

BinaryOperatorFun disjunction[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Disjunction<Cm::Sym::BoolValue>, NotSupported, NotSupported,
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Conjunction(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_and<typename ValueT::OperandType>());
}

BinaryOperatorFun conjunction[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Conjunction<Cm::Sym::BoolValue>, NotSupported, NotSupported,
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

void EvaluateBinOp(Cm::Sym::ValueType targetType, Cm::Sym::EvaluationStack& stack, BinaryOperatorFun* fun, bool cast, const Span& span)
{
    std::unique_ptr<Cm::Sym::Value> right(stack.Pop());
    std::unique_ptr<Cm::Sym::Value> left(stack.Pop());
    Cm::Sym::ValueType leftType = left->GetValueType();
    Cm::Sym::ValueType rightType = right->GetValueType();
    Cm::Sym::ValueType commonType = GetCommonType(leftType, rightType);
    Cm::Sym::ValueType operationType = commonType;
    if (targetType > commonType)
    {
        operationType = targetType;
    }
    std::unique_ptr<Cm::Sym::Value> left_(left->As(operationType, cast, span));
    std::unique_ptr<Cm::Sym::Value> right_(right->As(operationType, cast, span));
    BinaryOperatorFun operation = fun[uint8_t(operationType)];
    stack.Push(operation(left_.get(), right_.get(), span));
}

template <typename ValueT, typename Op>
Cm::Sym::Value* BinaryPredEvaluate(Cm::Sym::Value* left, Cm::Sym::Value* right, Op op)
{
    ValueT* left_ = static_cast<ValueT*>(left);
    ValueT* right_ = static_cast<ValueT*>(right);
    return new Cm::Sym::BoolValue(op(left_->Value(), right_->Value()));
}

template<typename ValueT>
Cm::Sym::Value* Equal(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::equal_to<typename ValueT::OperandType>());
}

BinaryOperatorFun equal[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Equal<Cm::Sym::BoolValue>, Equal<Cm::Sym::CharValue>, Equal<Cm::Sym::WCharValue>, Equal<Cm::Sym::UCharValue>,
    Equal<Cm::Sym::SByteValue>, Equal<Cm::Sym::ByteValue>, Equal<Cm::Sym::ShortValue>, Equal<Cm::Sym::UShortValue>, Equal<Cm::Sym::IntValue>, Equal<Cm::Sym::UIntValue>, 
    Equal<Cm::Sym::LongValue>, Equal<Cm::Sym::ULongValue>, Equal<Cm::Sym::FloatValue>, Equal<Cm::Sym::DoubleValue>, NotSupported, NotSupported, Equal<Cm::Sym::CharPtrValue>
};

template<typename ValueT>
Cm::Sym::Value* NotEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::not_equal_to<typename ValueT::OperandType>());
}

BinaryOperatorFun notEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotEqual<Cm::Sym::BoolValue>, NotEqual<Cm::Sym::CharValue>, NotEqual<Cm::Sym::WCharValue>, NotEqual<Cm::Sym::UCharValue>,
    NotEqual<Cm::Sym::SByteValue>, NotEqual<Cm::Sym::ByteValue>, NotEqual<Cm::Sym::ShortValue>, NotEqual<Cm::Sym::UShortValue>, NotEqual<Cm::Sym::IntValue>, NotEqual<Cm::Sym::UIntValue>, 
    NotEqual<Cm::Sym::LongValue>, NotEqual<Cm::Sym::ULongValue>, NotEqual<Cm::Sym::FloatValue>, NotEqual<Cm::Sym::DoubleValue>, NotSupported, NotSupported, NotEqual<Cm::Sym::CharPtrValue>
};

template<typename ValueT>
Cm::Sym::Value* Less(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less<typename ValueT::OperandType>());
}

BinaryOperatorFun less[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Less<Cm::Sym::BoolValue>, Less<Cm::Sym::CharValue>, Less<Cm::Sym::WCharValue>, Less<Cm::Sym::UCharValue>,
    Less<Cm::Sym::SByteValue>, Less<Cm::Sym::ByteValue>, Less<Cm::Sym::ShortValue>, Less<Cm::Sym::UShortValue>, Less<Cm::Sym::IntValue>, Less<Cm::Sym::UIntValue>, 
    Less<Cm::Sym::LongValue>, Less<Cm::Sym::ULongValue>, Less<Cm::Sym::FloatValue>, Less<Cm::Sym::DoubleValue>, NotSupported, NotSupported, Less<Cm::Sym::CharPtrValue>
};

template<typename ValueT>
Cm::Sym::Value* Greater(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater<typename ValueT::OperandType>());
}

BinaryOperatorFun greater[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Greater<Cm::Sym::BoolValue>, Greater<Cm::Sym::CharValue>, Greater<Cm::Sym::WCharValue>, Greater<Cm::Sym::UCharValue>,
    Greater<Cm::Sym::SByteValue>, Greater<Cm::Sym::ByteValue>, Greater<Cm::Sym::ShortValue>, Greater<Cm::Sym::UShortValue>, Greater<Cm::Sym::IntValue>, Greater<Cm::Sym::UIntValue>, 
    Greater<Cm::Sym::LongValue>, Greater<Cm::Sym::ULongValue>, Greater<Cm::Sym::FloatValue>, Greater<Cm::Sym::DoubleValue>, NotSupported, NotSupported, Greater<Cm::Sym::CharPtrValue>
};

template<typename ValueT>
Cm::Sym::Value* LessOrEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less_equal<typename ValueT::OperandType>());
}

BinaryOperatorFun lessOrEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, LessOrEqual<Cm::Sym::BoolValue>, LessOrEqual<Cm::Sym::CharValue>, LessOrEqual<Cm::Sym::WCharValue>, LessOrEqual<Cm::Sym::UCharValue>,
    LessOrEqual<Cm::Sym::SByteValue>, LessOrEqual<Cm::Sym::ByteValue>, LessOrEqual<Cm::Sym::ShortValue>, LessOrEqual<Cm::Sym::UShortValue>, LessOrEqual<Cm::Sym::IntValue>, LessOrEqual<Cm::Sym::UIntValue>, 
    LessOrEqual<Cm::Sym::LongValue>, LessOrEqual<Cm::Sym::ULongValue>, LessOrEqual<Cm::Sym::FloatValue>, LessOrEqual<Cm::Sym::DoubleValue>, NotSupported, NotSupported, LessOrEqual<Cm::Sym::CharPtrValue>
};

template<typename ValueT>
Cm::Sym::Value* GreaterOrEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater_equal<typename ValueT::OperandType>());
}

BinaryOperatorFun greaterOrEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, GreaterOrEqual<Cm::Sym::BoolValue>, GreaterOrEqual<Cm::Sym::CharValue>, GreaterOrEqual<Cm::Sym::WCharValue>, GreaterOrEqual<Cm::Sym::UCharValue>,
    GreaterOrEqual<Cm::Sym::SByteValue>, GreaterOrEqual<Cm::Sym::ByteValue>, GreaterOrEqual<Cm::Sym::ShortValue>, GreaterOrEqual<Cm::Sym::UShortValue>, GreaterOrEqual<Cm::Sym::IntValue>, GreaterOrEqual<Cm::Sym::UIntValue>, 
    GreaterOrEqual<Cm::Sym::LongValue>, GreaterOrEqual<Cm::Sym::ULongValue>, GreaterOrEqual<Cm::Sym::FloatValue>, GreaterOrEqual<Cm::Sym::DoubleValue>, NotSupported, NotSupported, GreaterOrEqual<Cm::Sym::CharPtrValue>
};

template <typename T>
void DefaultConstructor(VariableValueSymbol& variable, Cm::Sym::EvaluationStack& stack)
{
    variable.SetValue(new T());
};

template <typename T>
void CopyConstructor(VariableValueSymbol& variable, Cm::Sym::EvaluationStack& stack)
{
    variable.SetValue(stack.Pop());
};

typedef void (*ConstructorFun)(VariableValueSymbol&, Cm::Sym::EvaluationStack&);

template <Cm::Sym::ValueType valueType>
void NotSupported(VariableValueSymbol&, Cm::Sym::EvaluationStack&)
{
    throw Cm::Core::Exception("operation not supported for type " + ValueTypeStr(valueType));
}

ConstructorFun defaultCtor[uint8_t(Cm::Sym::ValueType::max)] = 
{
    NotSupported<Cm::Sym::ValueType::none>, DefaultConstructor<Cm::Sym::BoolValue>, DefaultConstructor<Cm::Sym::CharValue>, DefaultConstructor<Cm::Sym::WCharValue>, 
    DefaultConstructor<Cm::Sym::UCharValue>, DefaultConstructor<Cm::Sym::SByteValue>, DefaultConstructor<Cm::Sym::ByteValue>, DefaultConstructor<Cm::Sym::ShortValue>, 
    DefaultConstructor<Cm::Sym::UShortValue>, DefaultConstructor<Cm::Sym::IntValue>, DefaultConstructor<Cm::Sym::UIntValue>, DefaultConstructor<Cm::Sym::LongValue>,
    DefaultConstructor<Cm::Sym::ULongValue>, DefaultConstructor<Cm::Sym::FloatValue>, DefaultConstructor<Cm::Sym::DoubleValue>, DefaultConstructor<Cm::Sym::NullValue>, 
    DefaultConstructor<Cm::Sym::StringValue>, DefaultConstructor<Cm::Sym::CharPtrValue>
};

ConstructorFun copyCtor[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported<Cm::Sym::ValueType::none>, CopyConstructor<Cm::Sym::BoolValue>, CopyConstructor<Cm::Sym::CharValue>, CopyConstructor<Cm::Sym::WCharValue>,
    CopyConstructor<Cm::Sym::UCharValue>, CopyConstructor<Cm::Sym::SByteValue>, CopyConstructor<Cm::Sym::ByteValue>, CopyConstructor<Cm::Sym::ShortValue>,
    CopyConstructor<Cm::Sym::UShortValue>, CopyConstructor<Cm::Sym::IntValue>, CopyConstructor<Cm::Sym::UIntValue>, CopyConstructor<Cm::Sym::LongValue>,
    CopyConstructor<Cm::Sym::ULongValue>, CopyConstructor<Cm::Sym::FloatValue>, CopyConstructor<Cm::Sym::DoubleValue>, CopyConstructor<Cm::Sym::NullValue>,
    CopyConstructor<Cm::Sym::StringValue>, CopyConstructor<Cm::Sym::CharPtrValue>
};

class FunctionGroupValue : public Cm::Sym::Value
{
public:
    FunctionGroupValue(Cm::Sym::FunctionGroupSymbol* functionGroup_, Cm::Sym::ContainerScope* qualifiedScope_);
    bool IsFuntionGroupValue() const override { return true; }
    Cm::Sym::FunctionGroupSymbol* FunctionGroup() const { return functionGroup; }
    Cm::Sym::ContainerScope* QualifiedScope() const { return qualifiedScope; }
    Cm::Sym::ValueType GetValueType() const override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Clone() const override { throw std::runtime_error("member function not applicable"); }
    void Read(Cm::Sym::Reader& reader) override { throw std::runtime_error("member function not applicable"); }
    void Write(Cm::Sym::Writer& writer) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* As(Cm::Sym::ValueType targetType, bool cast, const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    void Inc(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    void Dec(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Deref(const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    virtual Ir::Intf::Object* CreateIrObject() const override { throw std::runtime_error("member function not applicable"); }
private:
    Cm::Sym::FunctionGroupSymbol* functionGroup;
    Cm::Sym::ContainerScope* qualifiedScope;
};

FunctionGroupValue::FunctionGroupValue(Cm::Sym::FunctionGroupSymbol* functionGroup_, Cm::Sym::ContainerScope* qualifiedScope_) : functionGroup(functionGroup_), qualifiedScope(qualifiedScope_)
{
}

class VariableValue : public Cm::Sym::Value
{
public:
    VariableValue(VariableValueSymbol* variableValueSymbol_);
    bool IsVariableValue() const override { return true; }
    VariableValueSymbol* GetVariableValueSymbol() const { return variableValueSymbol; }
    Cm::Sym::ValueType GetValueType() const override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Clone() const override { throw std::runtime_error("member function not applicable"); }
    void Read(Cm::Sym::Reader& reader) override { throw std::runtime_error("member function not applicable"); }
    void Write(Cm::Sym::Writer& writer) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* As(Cm::Sym::ValueType targetType, bool cast, const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    void Inc(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    void Dec(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Deref(const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    virtual Ir::Intf::Object* CreateIrObject() const override { throw std::runtime_error("member function not applicable"); }
private:
    VariableValueSymbol* variableValueSymbol;
};

VariableValue::VariableValue(VariableValueSymbol* variableValueSymbol_) : variableValueSymbol(variableValueSymbol_)
{
}

class Evaluator : public Cm::Ast::Visitor
{
public:
    Evaluator(Cm::Sym::ValueType targetType_, bool cast, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, 
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, 
        EvaluationFlags flags_);
    Cm::Sym::Value* DoEvaluate(Cm::Ast::Node* value);
    void Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode) override;
    void Visit(Cm::Ast::AliasNode& aliasNode) override;
    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override;
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override;
    void BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
    void EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode) override;
    void BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode) override;
    void EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode) override;
    void BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode) override;
    void BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode) override;
    void BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode) override;
    void BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode) override;
    void BeginVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void EndVisit(Cm::Ast::DoStatementNode& doStatementNode) override;
    void BeginVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void Visit(Cm::Ast::BreakStatementNode& breakStatementNode) override;
    void Visit(Cm::Ast::ContinueStatementNode& continueStatementNode) override;
    void EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode) override;

    void Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode) override;
    void Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode) override;
    void Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode) override;
    void Visit(Cm::Ast::IntLiteralNode& intLiteralNode) override;
    void Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode) override;
    void Visit(Cm::Ast::LongLiteralNode& longLiteralNode) override;
    void Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode) override;
    void Visit(Cm::Ast::FloatLiteralNode& floatLitealNode) override;
    void Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(Cm::Ast::CharLiteralNode& charLiteralNode) override;
    void Visit(Cm::Ast::StringLiteralNode& stringLiteralNode) override;
    void Visit(Cm::Ast::WStringLiteralNode& wstringLiteralNode) override;
    void Visit(Cm::Ast::UStringLiteralNode& ustringLiteralNode) override;
    void Visit(Cm::Ast::NullLiteralNode& nullLiteralNode) override;

    void BeginVisit(Cm::Ast::EquivalenceNode& equivalenceNode) override;
    void BeginVisit(Cm::Ast::ImplicationNode& implicationNode) override;
    void EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode) override;
    void EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode) override;
    void EndVisit(Cm::Ast::BitOrNode& bitOrNode) override;
    void EndVisit(Cm::Ast::BitXorNode& bitXorNode) override;
    void EndVisit(Cm::Ast::BitAndNode& bitAndNode) override;
    void EndVisit(Cm::Ast::EqualNode& equalNode) override;
    void EndVisit(Cm::Ast::NotEqualNode& notEqualNode) override;
    void EndVisit(Cm::Ast::LessNode& lessNode) override;
    void EndVisit(Cm::Ast::GreaterNode& greaterNode) override;
    void EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode) override;
    void EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode) override;
    void EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode) override;
    void EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode) override;
    void EndVisit(Cm::Ast::AddNode& addNode) override;
    void EndVisit(Cm::Ast::SubNode& subNode) override;
    void EndVisit(Cm::Ast::MulNode& mulNode) override;
    void EndVisit(Cm::Ast::DivNode& divNode) override;
    void EndVisit(Cm::Ast::RemNode& remNode) override;
    void BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode) override;
    void EndVisit(Cm::Ast::PrefixIncNode& prefixIncNode) override;
    void BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode) override;
    void EndVisit(Cm::Ast::PrefixDecNode& prefixDecNode) override;
    void EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) override;
    void EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode) override;
    void EndVisit(Cm::Ast::NotNode& notNode) override;
    void EndVisit(Cm::Ast::ComplementNode& complementNode) override;
    void Visit(Cm::Ast::AddrOfNode& addrOfNode) override;
    void Visit(Cm::Ast::DerefNode& derefNode) override;
    void Visit(Cm::Ast::PostfixIncNode& postfixIncNode) override;
    void Visit(Cm::Ast::PostfixDecNode& postfixDecNode) override;
    void BeginVisit(Cm::Ast::DotNode& dotNode) override;
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
    void Visit(Cm::Ast::ArrowNode& arrowNode) override;
    void BeginVisit(Cm::Ast::InvokeNode& invokeNode) override;
    void EndVisit(Cm::Ast::InvokeNode& invokeNode) override;
    void Visit(Cm::Ast::IndexNode& indexNode) override;

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) override;
    void Visit(Cm::Ast::CastNode& castNode) override;
    void Visit(Cm::Ast::ConstructNode& constructNode) override;
    void Visit(Cm::Ast::NewNode& newNode) override;
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void Visit(Cm::Ast::ThisNode& thisNode) override;
    void Visit(Cm::Ast::BaseNode& baseNode) override;
    void Visit(Cm::Ast::TypeNameNode& typeNameNode) override;
private:
    Cm::Sym::ValueType targetType;
    std::stack<Cm::Sym::ValueType> targetTypeStack;
    bool cast;
    std::stack<bool> castStack;
    Cm::Sym::ContainerScope* qualifiedScope;
    std::stack<Cm::Sym::ContainerScope*> qualifiedScopeStack;
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* currentContainerScope;
    std::stack<Cm::Sym::ContainerScope*> containerScopeStack;
    Cm::Sym::FunctionSymbol* currentFunction;
    std::stack<Cm::Sym::FunctionSymbol*> functionStack;
    Cm::Sym::DeclarationBlock* currentDeclarationBlock;
    Cm::Sym::FunctionGroupSymbol* currentFunctionGroupSymbol;
    std::stack<Cm::Sym::FunctionGroupSymbol*> functionGroupSymbolStack;
    std::stack<Cm::Sym::DeclarationBlock*> declarationBlockStack;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes;
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> functionFileScopes;
    Cm::Core::ClassTemplateRepository& classTemplateRepository;
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    Cm::Sym::EvaluationStack stack;
    Cm::Sym::SymbolTypeSetId lookupId;
    Cm::Sym::LookupIdStack lookupIdStack;
    EvaluationFlags flags;
    bool returned;
    std::stack<bool> returnedStack;
    bool interrupted;
    bool breaked;
    bool continued;
    bool leaveLvalue;
    std::stack<bool> leaveLvalueStack;
    void EvaluateSymbol(Cm::Sym::Symbol* symbol);
    bool DontThrow() const { return (flags & EvaluationFlags::dontThrow) != EvaluationFlags::none; }
    
};

Evaluator::Evaluator(Cm::Sym::ValueType targetType_, bool cast_, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, 
    EvaluationFlags flags_) :
    Visitor(true, true), targetType(targetType_), cast(cast_), qualifiedScope(nullptr), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScopes(fileScopes_),
    currentDeclarationBlock(nullptr), currentFunction(nullptr), currentFunctionGroupSymbol(nullptr), classTemplateRepository(classTemplateRepository_), boundCompileUnit(boundCompileUnit_),
    lookupId(Cm::Sym::SymbolTypeSetId::lookupConstantEnumConstantAndVariableValueSymbols), flags(flags_), returned(false), interrupted(false), breaked(false), continued(false), leaveLvalue(false)
{
}

Cm::Sym::Value* Evaluator::DoEvaluate(Cm::Ast::Node* value)
{
    try
    {
        value->Accept(*this);
        if (interrupted) return nullptr;
        std::unique_ptr<Cm::Sym::Value> result(stack.Pop());
        result.reset(result->As(targetType, cast, value->GetSpan()));
        return result.release();
    }
    catch (...)
    {
        if (!DontThrow())
        {
            throw;
        }
    }
    return nullptr;
}

void Evaluator::Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode)
{
    functionFileScopes.back()->InstallNamespaceImport(currentContainerScope, &namespaceImportNode);
}

void Evaluator::Visit(Cm::Ast::AliasNode& aliasNode)
{
    functionFileScopes.back()->InstallAlias(currentContainerScope, &aliasNode);
}

void Evaluator::BeginVisit(Cm::Ast::FunctionNode& functionNode)
{
    for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
    {
        functionFileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope->Clone()));
    }
    functionFileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(new Cm::Sym::FileScope()));
    for (const std::unique_ptr<Cm::Ast::Node>& usingNode : currentFunction->GetUsingNodes())
    {
        usingNode->Accept(*this);
    }
    PushSkipContent(true);
    int n = functionNode.Parameters().Count();
    std::vector<std::unique_ptr<VariableValueSymbol>> receives;
    Cm::Sym::DeclarationBlock functionSymbols(functionNode.GetSpan(), "functionSymbols");
    functionSymbols.SetParent(currentContainerScope->Container());
    functionSymbols.GetContainerScope()->SetParent(currentContainerScope);
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = functionSymbols.GetContainerScope();
    int bn = int(currentFunctionGroupSymbol->BoundTemplateArguments().size());
    if (bn > 0)
    {
        for (int i = 0; i < bn; ++i)
        {
            Cm::Sym::TypeParameterSymbol* typeParam = currentFunction->TypeParameters()[i];
            Cm::Sym::BoundTypeParameterSymbol* boundTypeParam = new Cm::Sym::BoundTypeParameterSymbol(functionNode.GetSpan(), typeParam->Name());
            Cm::Sym::TypeSymbol* templateArgumentType = currentFunctionGroupSymbol->BoundTemplateArguments()[i];
            boundTypeParam->SetType(templateArgumentType);
            functionSymbols.AddSymbol(boundTypeParam);
        }
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
        Cm::Sym::TypeSymbol* returnType = ResolveType(boundCompileUnit.SymbolTable(), currentContainerScope, fscopes, boundCompileUnit.ClassTemplateRepository(), boundCompileUnit, 
            functionNode.ReturnTypeExpr());
        currentFunction->SetReturnType(returnType);
    }
    currentFunctionGroupSymbol = nullptr;
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::Sym::Value> arg(stack.Pop());
        Cm::Ast::ParameterNode* param = functionNode.Parameters()[i];
        std::string id;
        if (param->Id())
        {
            id = param->Id()->Str();
        }
        else
        {
            id = "__parameter" + std::to_string(i);
        }
        VariableValueSymbol* var = new VariableValueSymbol(param->GetSpan(), id, arg.release());
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
        var->SetType(ResolveType(boundCompileUnit.SymbolTable(), currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, param->TypeExpr()));
        var->SetOwned();
        receives.push_back(std::unique_ptr<VariableValueSymbol>(var));
        functionSymbols.AddSymbol(var);
    }
    returnedStack.push(returned);
    returned = false;
    functionNode.Body()->Accept(*this);
    if (breaked)
    {
        throw Cm::Core::Exception("break not inside a while, do or for statement", functionNode.GetSpan());
    }
    if (continued)
    {
        throw Cm::Core::Exception("continue not inside a while, do or for statement", functionNode.GetSpan());
    }
    returned = returnedStack.top();
    returnedStack.pop();
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void Evaluator::EndVisit(Cm::Ast::FunctionNode& functionNode)
{
    PopSkipContent();
}

void Evaluator::BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    PushSkipContent(true);
    if (returned) return;
    if (interrupted) return;
    Cm::Sym::DeclarationBlock blockVars(compoundStatementNode.GetSpan(), "blockVars");
    blockVars.SetParent(currentContainerScope->Container());
    blockVars.GetContainerScope()->SetParent(currentContainerScope);
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = blockVars.GetContainerScope();
    declarationBlockStack.push(currentDeclarationBlock);
    currentDeclarationBlock = &blockVars;
    PushSkipContent(false);
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        compoundStatementNode.Statements()[i]->Accept(*this);
        if (returned || breaked || continued || interrupted)
        {
            PopSkipContent();
            currentContainerScope = containerScopeStack.top();
            containerScopeStack.pop();
            return;
        }
    }
    PopSkipContent();
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void Evaluator::EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode)
{
    PopSkipContent();
}

void Evaluator::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    PushSkipContent(true);
    PushSkipArguments(true);
    if (returned) return;
    if (interrupted) return;
    std::vector<Cm::Core::Argument> args;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
    Cm::Sym::TypeSymbol* type = ResolveType(boundCompileUnit.SymbolTable(), currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, constructionStatementNode.TypeExpr());
    Cm::Sym::SymbolType symbolType = type->GetSymbolType();
    Cm::Sym::ValueType valueType = Cm::Sym::GetValueTypeFor(symbolType, DontThrow());
    if (valueType == Cm::Sym::ValueType::none) { interrupted = true; return; }
    Cm::Sym::TypeSymbol* ptrType = boundCompileUnit.SymbolTable().GetTypeRepository().MakePointerType(type, constructionStatementNode.GetSpan());
    args.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::lvalue, ptrType));
    Cm::Ast::NodeList& argumentExprs = constructionStatementNode.Arguments();
    std::vector<std::unique_ptr<Cm::Sym::Value>> arguments;
    std::unique_ptr<VariableValueSymbol> var(new VariableValueSymbol(constructionStatementNode.GetSpan(), constructionStatementNode.Id()->Str()));
    var->SetType(type);
    int n = argumentExprs.Count();
    for (int i = 0; i < n; ++i)
    {
        argumentExprs[i]->Accept(*this);
        if (interrupted) return;
        std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
        arguments.push_back(std::unique_ptr<Cm::Sym::Value>(value->As(valueType, false, constructionStatementNode.GetSpan())));
        args.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, type));
    }
    Cm::Sym::FunctionLookupSet functionLookups;
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, boundCompileUnit.SymbolTable().GlobalScope()));
    functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
    std::vector<Cm::Sym::FunctionSymbol*> conversions;
    Cm::Sym::FunctionSymbol* ctor = ResolveOverload(currentContainerScope, boundCompileUnit, "@constructor", args, functionLookups, constructionStatementNode.GetSpan(), conversions);
    ConstructorFun* ctorFun = nullptr;
    switch (ctor->GetBcuItemType())
    {
        case Cm::Sym::BcuItemType::bcuDefaultCtor: ctorFun = &defaultCtor[std::uint8_t(valueType)]; break;
        case Cm::Sym::BcuItemType::bcuCopyCtor: ctorFun = &copyCtor[std::uint8_t(valueType)]; break;
        default: 
        {
            if (DontThrow())
            {
                interrupted = true;
                return;
            }
            else
            {
                throw Cm::Core::Exception("cannot evaluate constructor '" + ctor->FullName() + "' statically", constructionStatementNode.GetSpan());
            }
        }
    }
    for (int i = 0; i < n; ++i)
    {
        stack.Push(arguments[i].release());
    }
    (*ctorFun)(*var, stack);
    currentDeclarationBlock->AddSymbol(var.release());
}

void Evaluator::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    PopSkipArguments();
    PopSkipContent();
}

void Evaluator::BeginVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    PushVisitExpressions(false);
    leaveLvalueStack.push(leaveLvalue);
    leaveLvalue = true;
    assignmentStatementNode.TargetExpr()->Accept(*this);
    if (interrupted) return;
    leaveLvalue = leaveLvalueStack.top();
    leaveLvalueStack.pop();
    std::unique_ptr<Cm::Sym::Value> targetValue(stack.Pop());
    if (targetValue->IsVariableValue())
    {
        VariableValue* variableValue = static_cast<VariableValue*>(targetValue.get());
        VariableValueSymbol* variableValueSymbol = variableValue->GetVariableValueSymbol();
        Cm::Sym::TypeSymbol* type = variableValueSymbol->GetType();
        Cm::Sym::SymbolType symbolType = type->GetSymbolType();
        targetTypeStack.push(targetType);
        targetType = Cm::Sym::GetValueTypeFor(symbolType, DontThrow());
        if (targetType == Cm::Sym::ValueType::none) { interrupted = true; return; }
        assignmentStatementNode.SourceExpr()->Accept(*this);
        targetType = targetTypeStack.top();
        targetTypeStack.pop();
        variableValueSymbol->SetValue(stack.Pop()->As(targetType, false, assignmentStatementNode.SourceExpr()->GetSpan()));
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("assignment needs a variable", assignmentStatementNode.TargetExpr()->GetSpan());
        }
    }
}

void Evaluator::EndVisit(Cm::Ast::AssignmentStatementNode& assignmentStatementNode)
{
    PopVisitExpressions();
}

void Evaluator::BeginVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    PushSkipContent(true);
    targetTypeStack.push(targetType);
    targetType = Cm::Sym::ValueType::boolValue;
}

void Evaluator::EndVisit(Cm::Ast::ConditionalStatementNode& conditionalStatementNode)
{
    targetType = targetTypeStack.top();
    targetTypeStack.pop();
    PopSkipContent();
    if (returned) return;
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, conditionalStatementNode.GetSpan()));
    Cm::Sym::BoolValue* cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    if (cond->Value())
    {
        conditionalStatementNode.ThenS()->Accept(*this);
    }
    else
    {
        if (conditionalStatementNode.ElseS())
        {
            conditionalStatementNode.ElseS()->Accept(*this);
        }
    }
}

void Evaluator::EndVisit(Cm::Ast::ReturnStatementNode& returnStatementNode)
{
    if (returned) return;
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    Cm::Sym::SymbolType returnSymbolType = currentFunction->GetReturnType()->GetSymbolType();
    Cm::Sym::ValueType returnValueType = Cm::Sym::GetValueTypeFor(returnSymbolType, DontThrow());
    if (returnValueType == Cm::Sym::ValueType::none) { interrupted = true; return; }
    stack.Push(value->As(returnValueType, false, returnStatementNode.GetSpan()));
    returned = true;
}

void Evaluator::BeginVisit(Cm::Ast::SwitchStatementNode& switchStatementNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", switchStatementNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::CaseStatementNode& caseStatementNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", caseStatementNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::DefaultStatementNode& defaultStatementNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", defaultStatementNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::GotoCaseStatementNode& gotoCaseStatementNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", gotoCaseStatementNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", gotoDefaultStatementNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    PushSkipContent(true);
    targetTypeStack.push(targetType);
    targetType = Cm::Sym::ValueType::boolValue;
}

void Evaluator::EndVisit(Cm::Ast::WhileStatementNode& whileStatementNode)
{
    targetType = targetTypeStack.top();
    targetTypeStack.pop();
    PopSkipContent();
    if (returned) return;
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, whileStatementNode.GetSpan()));
    Cm::Sym::BoolValue* cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    while (cond->Value())
    {
        whileStatementNode.Statement()->Accept(*this);
        if (returned) return;
        if (interrupted) return;
        if (breaked)
        {
            breaked = false;
            break;
        }
        if (continued)
        {
            continued = false;
        }
        targetTypeStack.push(targetType);
        targetType = Cm::Sym::ValueType::boolValue;
        whileStatementNode.Condition()->Accept(*this);
        targetType = targetTypeStack.top();
        targetTypeStack.pop();
        if (returned) return;
        if (interrupted) return;
        value.reset(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, whileStatementNode.GetSpan()));
        cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    }
}

void Evaluator::BeginVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    PushSkipContent(true);
    PushVisitExpressions(false);
}

void Evaluator::EndVisit(Cm::Ast::DoStatementNode& doStatementNode)
{
    PopVisitExpressions();
    PopSkipContent();
    if (returned) return;
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value;
    Cm::Sym::BoolValue* cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    do
    {
        doStatementNode.Statement()->Accept(*this);
        if (returned) return;
        if (interrupted) return;
        if (breaked)
        {
            breaked = false;
            break;
        }
        if (continued)
        {
            continued = false;
        }
        targetTypeStack.push(targetType);
        targetType = Cm::Sym::ValueType::boolValue;
        doStatementNode.Condition()->Accept(*this);
        targetType = targetTypeStack.top();
        targetTypeStack.pop();
        if (returned) return;
        if (interrupted) return;
        value.reset(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, doStatementNode.GetSpan()));
        cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    } 
    while (cond->Value());
}

void Evaluator::BeginVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    PushSkipContent(true);
    PushVisitExpressions(false);
}

void Evaluator::EndVisit(Cm::Ast::ForStatementNode& forStatementNode)
{
    PopVisitExpressions();
    PopSkipContent();
    if (returned) return;
    if (interrupted) return;
    Cm::Sym::DeclarationBlock forVars(forStatementNode.GetSpan(), "forVars");
    forVars.SetParent(currentContainerScope->Container());
    forVars.GetContainerScope()->SetParent(currentContainerScope);
    containerScopeStack.push(currentContainerScope);
    currentContainerScope = forVars.GetContainerScope();
    declarationBlockStack.push(currentDeclarationBlock);
    currentDeclarationBlock = &forVars;
    forStatementNode.Init()->Accept(*this);
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value;
    if (forStatementNode.HasCondition())
    {
        targetTypeStack.push(targetType);
        targetType = Cm::Sym::ValueType::boolValue;
        forStatementNode.Condition()->Accept(*this);
        targetType = targetTypeStack.top();
        targetTypeStack.pop();
        if (interrupted) return;
        value.reset(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, forStatementNode.GetSpan()));
    }
    Cm::Sym::BoolValue* cond = static_cast<Cm::Sym::BoolValue*>(value.get());
    while (!cond || cond->Value())
    {
        forStatementNode.Action()->Accept(*this);
        if (interrupted) return;
        if (returned)
        {
            currentContainerScope = containerScopeStack.top();
            containerScopeStack.pop();
            return;
        }
        if (breaked)
        {
            breaked = false;
            break;
        }
        if (continued)
        {
            continued = false;
        }
        if (forStatementNode.HasIncrement())
        {
            forStatementNode.Increment()->Accept(*this);
            if (interrupted) return;
            std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
        }
        if (forStatementNode.HasCondition())
        {
            targetTypeStack.push(targetType);
            targetType = Cm::Sym::ValueType::boolValue;
            forStatementNode.Condition()->Accept(*this);
            targetType = targetTypeStack.top();
            targetTypeStack.pop();
            if (interrupted) return;
            value.reset(stack.Pop()->As(Cm::Sym::ValueType::boolValue, false, forStatementNode.GetSpan()));
            cond = static_cast<Cm::Sym::BoolValue*>(value.get());
        }
    }
    currentContainerScope = containerScopeStack.top();
    containerScopeStack.pop();
}

void Evaluator::Visit(Cm::Ast::BreakStatementNode& breakStatementNode)
{
    breaked = true;
}

void Evaluator::Visit(Cm::Ast::ContinueStatementNode& continueStatementNode)
{
    continued = true;
}

void Evaluator::EndVisit(Cm::Ast::SimpleStatementNode& simpleStatementNode)
{
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
}

void Evaluator::Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::BoolValue(booleanLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::SByteValue(sbyteLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::ByteValue(byteLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::ShortValue(shortLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::UShortValue(ushortLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::IntValue(intLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::UIntValue(uintLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::LongValue(longLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::ULongValue(ulongLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::FloatValue(floatLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::DoubleValue(doubleLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::CharValue(charLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::CharPtrValue(stringLiteralNode.Value().c_str()));
}

void Evaluator::Visit(Cm::Ast::WStringLiteralNode& wstringLiteralNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", wstringLiteralNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::UStringLiteralNode& ustringLiteralNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", ustringLiteralNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    if (returned) return;
    if (interrupted) return;
    stack.Push(new Cm::Sym::NullValue());
}

void Evaluator::BeginVisit(Cm::Ast::EquivalenceNode& equivalenceNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("operation not supported", equivalenceNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::ImplicationNode& implicationNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("operation not supported", implicationNode.GetSpan());
    }
}

void Evaluator::EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, disjunction, cast, disjunctionNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    if (returned) return; 
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, conjunction, cast, conjunctionNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitOrNode& bitOrNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitOr, cast, bitOrNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitXorNode& bitXorNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitXor, cast, bitXorNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitAndNode& bitAndNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitAnd, cast, bitAndNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::EqualNode& equalNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, equal, cast, equalNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::NotEqualNode& notEqualNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, notEqual, cast, notEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::LessNode& lessNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, less, cast, lessNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::GreaterNode& greaterNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greater, cast, greaterNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, lessOrEqual, cast, lessOrEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greaterOrEqual, cast, greaterOrEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftLeft, cast, shiftLeftNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftRight, cast, shiftRightNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::AddNode& addNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, add, cast, addNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::SubNode& subNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, sub, cast, subNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::MulNode& mulNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, mul, cast, mulNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::DivNode& divNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, div, cast, divNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::RemNode& remNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, rem, cast, remNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode)
{
    if (returned) return;
    if (interrupted) return;
    leaveLvalueStack.push(leaveLvalue);
    leaveLvalue = true;
}

void Evaluator::EndVisit(Cm::Ast::PrefixIncNode& prefixIncNode)
{
    if (returned) return;
    if (interrupted) return;
    leaveLvalue = leaveLvalueStack.top();
    leaveLvalueStack.pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsVariableValue())
    {
        VariableValue* variableValue = static_cast<VariableValue*>(value.get());
        variableValue->GetVariableValueSymbol()->GetValue()->Inc(prefixIncNode.GetSpan());
        stack.Push(variableValue->GetVariableValueSymbol()->GetValue()->Clone());
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("prefix operator++ needs a variable", prefixIncNode.Child()->GetSpan());
        }
    }
}

void Evaluator::BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode)
{
    if (returned) return;
    if (interrupted) return;
    leaveLvalueStack.push(leaveLvalue);
    leaveLvalue = true;
}

void Evaluator::EndVisit(Cm::Ast::PrefixDecNode& prefixDecNode)
{
    if (returned) return;
    if (interrupted) return;
    leaveLvalue = leaveLvalueStack.top();
    leaveLvalueStack.pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsVariableValue())
    {
        VariableValue* variableValue = static_cast<VariableValue*>(value.get());
        variableValue->GetVariableValueSymbol()->GetValue()->Dec(prefixDecNode.GetSpan());
        stack.Push(variableValue->GetVariableValueSymbol()->GetValue()->Clone());
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("prefix operator-- needs a variable", prefixDecNode.Child()->GetSpan());
        }
    }
}

void Evaluator::EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, unaryPlus, cast, unaryPlusNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, negate, cast, unaryMinusNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::NotNode& notNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, not_, cast, notNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ComplementNode& complementNode)
{
    if (returned) return;
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, complement, cast, complementNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::AddrOfNode& addrOfNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", addrOfNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::DerefNode& derefNode)
{
    if (returned) return;
    if (interrupted) return;
    derefNode.Subject()->Accept(*this);
    if (interrupted) return;
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    stack.Push(value->Deref(derefNode.Subject()->GetSpan()));
}

void Evaluator::Visit(Cm::Ast::PostfixIncNode& postfixIncNode)
{
    if (returned) return;
    if (interrupted) return;
    postfixIncNode.Child()->Accept(*this);
    if (interrupted) return;
    leaveLvalueStack.push(leaveLvalue);
    leaveLvalue = true;
    postfixIncNode.Child()->Accept(*this);
    if (interrupted) return;
    leaveLvalue = leaveLvalueStack.top();
    leaveLvalueStack.pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsVariableValue())
    {
        VariableValue* variableValue = static_cast<VariableValue*>(value.get());
        variableValue->GetVariableValueSymbol()->GetValue()->Inc(postfixIncNode.GetSpan());
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("postfix operator++ needs a variable", postfixIncNode.Child()->GetSpan());
        }
    }
}

void Evaluator::Visit(Cm::Ast::PostfixDecNode& postfixDecNode)
{
    if (returned) return;
    if (interrupted) return;
    postfixDecNode.Child()->Accept(*this);
    if (interrupted) return;
    leaveLvalueStack.push(leaveLvalue);
    leaveLvalue = true;
    postfixDecNode.Child()->Accept(*this);
    if (interrupted) return;
    leaveLvalue = leaveLvalueStack.top();
    leaveLvalueStack.pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsVariableValue())
    {
        VariableValue* variableValue = static_cast<VariableValue*>(value.get());
        variableValue->GetVariableValueSymbol()->GetValue()->Dec(postfixDecNode.GetSpan());
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("postfix operator-- needs a variable", postfixDecNode.Child()->GetSpan());
        }
    }
}

class ScopedValue : public Cm::Sym::Value
{
public:
    ScopedValue(Cm::Sym::ContainerSymbol* containerSymbol_) : containerSymbol(containerSymbol_) {}
    bool IsScopedValue() const override { return true; }
    Cm::Sym::ContainerSymbol* ContainerSymbol() const { return containerSymbol; }
    Cm::Sym::ValueType GetValueType() const override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Clone() const override { throw std::runtime_error("member function not applicable"); }
    void Read(Cm::Sym::Reader& reader) override { throw std::runtime_error("member function not applicable"); }
    void Write(Cm::Sym::Writer& writer) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* As(Cm::Sym::ValueType targetType, bool cast, const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    void Inc(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    void Dec(const Span& span) override { throw std::runtime_error("member function not applicable"); }
    Cm::Sym::Value* Deref(const Span& span) const override { throw std::runtime_error("member function not applicable"); }
    virtual Ir::Intf::Object* CreateIrObject() const override { throw std::runtime_error("member function not applicable"); }
private:
    Cm::Sym::ContainerSymbol* containerSymbol;
};

void Evaluator::BeginVisit(Cm::Ast::DotNode& dotNode)
{
    if (returned) return;
    if (interrupted) return;
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupContainerSymbols;
}

void Evaluator::EndVisit(Cm::Ast::DotNode& dotNode)
{
    if (returned) return;
    if (interrupted) return;
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsScopedValue())
    {
        ScopedValue* scopedValue = static_cast<ScopedValue*>(value.get());
        Cm::Sym::ContainerSymbol* containerSymbol = scopedValue->ContainerSymbol();
        Cm::Sym::ContainerScope* scope = containerSymbol->GetContainerScope();
        Cm::Sym::Symbol* symbol = scope->Lookup(dotNode.MemberStr(), lookupId);
        if (symbol)
        {
            qualifiedScopeStack.push(qualifiedScope);
            qualifiedScope = containerSymbol->GetContainerScope();
            EvaluateSymbol(symbol);
            qualifiedScope = qualifiedScopeStack.top();
            qualifiedScopeStack.pop();
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + containerSymbol->FullName() + "' does not have member '" + dotNode.MemberStr() + "'", dotNode.GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("expression '" + dotNode.Subject()->FullName() + "' must denote a namespace, class type or enumerated type", dotNode.Subject()->GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::ArrowNode& arrowNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", arrowNode.GetSpan());
    }
}

void Evaluator::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    PushSkipContent(true);
    PushSkipArguments(true);
    functionGroupSymbolStack.push(currentFunctionGroupSymbol);
    currentFunctionGroupSymbol = nullptr;
    if (returned) return;
    if (interrupted) return;
    std::vector<Cm::Core::Argument> args;
    Cm::Ast::NodeList& argumentExprs = invokeNode.Arguments();
    std::vector<std::unique_ptr<Cm::Sym::Value>> arguments;
    int n = argumentExprs.Count();
    for (int i = 0; i < n; ++i)
    {
        argumentExprs[i]->Accept(*this);
        if (interrupted) return;
        Cm::Sym::Value* value = stack.Pop();
        arguments.push_back(std::unique_ptr<Cm::Sym::Value>(value));
        Cm::Sym::TypeSymbol* type = Cm::Sym::GetTypeSymbol(value->GetValueType(), symbolTable);
        if (type)
        {
            args.push_back(Cm::Core::Argument(Cm::Core::ArgumentCategory::rvalue, type));
        }
        else
        {
            if (DontThrow())
            {
                interrupted = true;
                return;
            }
            else
            {
                throw Cm::Core::Exception("type not found", argumentExprs[i]->GetSpan());
            }
        }
    }
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupInvokeSubject;
    invokeNode.Subject()->Accept(*this);
    if (interrupted) return;
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsFuntionGroupValue())
    {
        Cm::Sym::FunctionLookupSet functionLookups;
        FunctionGroupValue* functionGroupValue = static_cast<FunctionGroupValue*>(value.get());
        Cm::Sym::FunctionGroupSymbol* functionGroup = functionGroupValue->FunctionGroup();
        if (functionGroupValue->QualifiedScope())
        {
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base, functionGroupValue->QualifiedScope()));
        }
        else
        {
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent, functionGroup->GetContainerScope()));
            functionLookups.Add(Cm::Sym::FunctionLookup(Cm::Sym::ScopeLookup::fileScopes, nullptr));
        }
        std::vector<Cm::Sym::FunctionSymbol*> conversions;
        Cm::Sym::FunctionSymbol* fun = ResolveOverload(currentContainerScope, boundCompileUnit, functionGroup->Name(), args, functionLookups, invokeNode.GetSpan(), conversions,
            Cm::Sym::ConversionType::implicit, functionGroup->BoundTemplateArguments(), OverloadResolutionFlags::dontInstantiate);
        if (fun->IsConstExpr())
        {
            Cm::Ast::Node* node = boundCompileUnit.ConstExprFunctionRepository().GetNodeFor(fun);
            if (!node->IsFunctionNode())
            {
                if (DontThrow())
                {
                    interrupted = true;
                    return;
                }
                else
                {
                    throw std::runtime_error("function node expected");
                }
            }
            Cm::Ast::FunctionNode* functionNode = static_cast<Cm::Ast::FunctionNode*>(node);
            const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
            CheckFunctionReturnPaths(boundCompileUnit.SymbolTable(), currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, fun, functionNode);
            for (int i = 0; i < n; ++i)
            {
                Cm::Sym::FunctionSymbol* conversion = conversions[i];
                if (conversion)
                {
                    Cm::Sym::SymbolType symbolType = conversion->GetTargetType()->GetSymbolType();
                    Cm::Sym::ValueType targetType = Cm::Sym::GetValueTypeFor(symbolType, DontThrow());
                    if (targetType == Cm::Sym::ValueType::none) { interrupted = true; return; }
                    stack.Push(arguments[i]->As(targetType, false, invokeNode.GetSpan()));
                }
                else
                {
                    stack.Push(arguments[i].release());
                }
            }
            functionStack.push(currentFunction);
            currentFunction = fun;
            currentFunctionGroupSymbol = functionGroup;
            functionNode->Accept(*this);
            if (interrupted) return;
            currentFunction = functionStack.top();
            functionStack.pop();
        }
        else
        {
            if (fun->IsIntrinsic())
            {
                Cm::Sym::IntrinsicFunction* intrinsic = fun->GetIntrinsic();
                if (intrinsic)
                {
                    intrinsic->Evaluate(stack, currentFunctionGroupSymbol->BoundTemplateArguments());
                }
                else
                {
                    throw Cm::Core::Exception("intrinsic function implementation not set", fun->GetSpan());
                }
            }
            else
            {
                if (DontThrow())
                {
                    interrupted = true;
                    return;
                }
                else
                {
                    throw Cm::Core::Exception("constexpr or intrinsic function expected", invokeNode.Subject()->GetSpan());
                }
            }
        }
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("function group expected", invokeNode.Subject()->GetSpan());
        }
    }
    currentFunctionGroupSymbol = functionGroupSymbolStack.top();
    functionGroupSymbolStack.pop();
}

void Evaluator::EndVisit(Cm::Ast::InvokeNode& invokeNode)
{
    PopSkipArguments();
    PopSkipContent();
}

void Evaluator::Visit(Cm::Ast::IndexNode& indexNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", indexNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::SizeOfNode& sizeOfNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("not implemented yet", sizeOfNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::CastNode& castNode)
{
    if (returned) return;
    if (interrupted) return;
    Cm::Ast::Node* targetTypeExpr = castNode.TargetTypeExpr();
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, targetTypeExpr);
    Cm::Sym::SymbolType symbolType = type->GetSymbolType();
    Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType, DontThrow());
    if (valueType == Cm::Sym::ValueType::none) { interrupted = true; return; }
    stack.Push(Evaluate(valueType, true, castNode.SourceExpr(), symbolTable, currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, flags));
}

void Evaluator::Visit(Cm::Ast::ConstructNode& constructNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", constructNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::NewNode& newNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", newNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupFunctionGroup;
    templateIdNode.Subject()->Accept(*this);
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsFuntionGroupValue())
    {
        FunctionGroupValue* functionGroupValue = static_cast<FunctionGroupValue*>(value.get());
        std::vector<Cm::Sym::TypeSymbol*> templateArguments;
        for (const std::unique_ptr<Cm::Ast::Node>& templateArgument : templateIdNode.TemplateArguments())
        {
            Cm::Sym::TypeSymbol* templateArgumentType = ResolveType(boundCompileUnit.SymbolTable(), currentContainerScope, boundCompileUnit.GetFileScopes(), boundCompileUnit.ClassTemplateRepository(),
                boundCompileUnit, templateArgument.get());
            templateArguments.push_back(templateArgumentType);
        }
        functionGroupValue->FunctionGroup()->SetBoundTemplateArguments(templateArguments);
        stack.Push(value.release());
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("function group expected", templateIdNode.Subject()->GetSpan());
        }
    }
}

void Evaluator::EvaluateSymbol(Cm::Sym::Symbol* symbol)
{
    if (returned) return;
    if (interrupted) return;
    if (symbol->IsContainerSymbol())
    {
        Cm::Sym::ContainerSymbol* containerSymbol = static_cast<Cm::Sym::ContainerSymbol*>(symbol);
        stack.Push(new ScopedValue(containerSymbol));
    }
    else if (symbol->IsConstantSymbol())
    {
        Cm::Sym::ConstantSymbol* constantSymbol = static_cast<Cm::Sym::ConstantSymbol*>(symbol);
        if (!constantSymbol->GetValue())
        {
            Cm::Ast::Node* node = symbolTable.GetNode(constantSymbol);
            if (node->IsConstantNode())
            {
                Cm::Ast::ConstantNode* constantNode = static_cast<Cm::Ast::ConstantNode*>(node);
                const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
                BindConstant(symbolTable, currentContainerScope, fscopes, classTemplateRepository, boundCompileUnit, constantNode);
            }
            else
            {
                if (DontThrow())
                {
                    interrupted = true;
                    return;
                }
                else
                {
                    throw std::runtime_error("node is not constant node");
                }
            }
        }
        stack.Push(constantSymbol->GetValue()->Clone());
    }
    else if (symbol->IsEnumConstantSymbol())
    {
        Cm::Sym::EnumConstantSymbol* enumConstantSymbol = static_cast<Cm::Sym::EnumConstantSymbol*>(symbol);
        if (!enumConstantSymbol->GetValue())
        {
            Cm::Ast::Node* node = symbolTable.GetNode(enumConstantSymbol);
            if (node->IsEnumConstantNode())
            {
                Cm::Ast::EnumConstantNode* enumConstantNode = static_cast<Cm::Ast::EnumConstantNode*>(node);
                Cm::Sym::ContainerScope* enumConstantContainerScope = symbolTable.GetContainerScope(enumConstantNode);
                const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
                BindEnumConstant(symbolTable, enumConstantContainerScope, fscopes, classTemplateRepository, boundCompileUnit, enumConstantNode);
            }
            else
            {
                if (DontThrow())
                {
                    interrupted = true;
                    return;
                }
                else
                {
                    throw std::runtime_error("node is not enumeration constant node");
                }
            }
        }
        stack.Push(enumConstantSymbol->GetValue()->Clone());
    }
    else if (symbol->IsVariableValueSymbol())
    {
        VariableValueSymbol* variableValueSymbol = static_cast<VariableValueSymbol*>(symbol);
        if (leaveLvalue)
        {
            stack.Push(new VariableValue(variableValueSymbol));
        }
        else
        {
            stack.Push(variableValueSymbol->GetValue()->Clone());
        }
    }
    else if (symbol->IsFunctionGroupSymbol())
    {
        Cm::Sym::FunctionGroupSymbol* functionGroup = static_cast<Cm::Sym::FunctionGroupSymbol*>(symbol);
        functionGroup->SetBoundTemplateArguments(std::vector<Cm::Sym::TypeSymbol*>());
        stack.Push(new FunctionGroupValue(functionGroup, qualifiedScope));
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("cannot evaluate statically", symbol->GetSpan());
        }
    }
}

void Evaluator::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    if (returned) return;
    if (interrupted) return;
    Cm::Sym::Symbol* symbol = currentContainerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, lookupId);
    if (!symbol)
    {
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fscopes = functionFileScopes.empty() ? fileScopes : functionFileScopes;
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fscopes)
        {
            symbol = fileScope->Lookup(identifierNode.Str(), lookupId);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        EvaluateSymbol(symbol);
    }
    else
    {
        if (DontThrow())
        {
            interrupted = true;
            return;
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
        }
    }
}

void Evaluator::Visit(Cm::Ast::ThisNode& thisNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", thisNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::BaseNode& baseNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", baseNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::TypeNameNode& typeNameNode)
{
    if (DontThrow())
    {
        interrupted = true;
        return;
    }
    else
    {
        throw Cm::Core::Exception("cannot evaluate statically", typeNameNode.GetSpan());
    }
}

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    return Evaluate(targetType, cast, value, symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, EvaluationFlags::none);
}

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope,
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
    EvaluationFlags flags)
{
    Evaluator evaluator(targetType, cast, symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, flags);
    return evaluator.DoEvaluate(value);
}

class BooleanEvaluator : public Cm::Ast::Visitor
{
public:
    BooleanEvaluator(bool cast, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
    bool DoEvaluate(Cm::Ast::Node* value);
    void Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode) override;
    void Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode) override;
    void Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode) override;
    void Visit(Cm::Ast::IntLiteralNode& intLiteralNode) override;
    void Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode) override;
    void Visit(Cm::Ast::LongLiteralNode& longLiteralNode) override;
    void Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode) override;
    void Visit(Cm::Ast::FloatLiteralNode& floatLitealNode) override;
    void Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(Cm::Ast::CharLiteralNode& charLiteralNode) override;
    void Visit(Cm::Ast::StringLiteralNode& stringLiteralNode) override;
    void Visit(Cm::Ast::WStringLiteralNode& wstringLiteralNode) override;
    void Visit(Cm::Ast::UStringLiteralNode& ustringLiteralNode) override;
    void Visit(Cm::Ast::NullLiteralNode& nullLiteralNode) override;

    void BeginVisit(Cm::Ast::ClassNode& classNode) override;
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;

    void BeginVisit(Cm::Ast::EquivalenceNode& equivalenceNode) override;
    void BeginVisit(Cm::Ast::ImplicationNode& implicationNode) override;
    void EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode) override;
    void EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode) override;
    void EndVisit(Cm::Ast::BitOrNode& bitOrNode) override;
    void EndVisit(Cm::Ast::BitXorNode& bitXorNode) override;
    void EndVisit(Cm::Ast::BitAndNode& bitAndNode) override;
    void EndVisit(Cm::Ast::EqualNode& equalNode) override;
    void EndVisit(Cm::Ast::NotEqualNode& notEqualNode) override;
    void EndVisit(Cm::Ast::LessNode& lessNode) override;
    void EndVisit(Cm::Ast::GreaterNode& greaterNode) override;
    void EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode) override;
    void EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode) override;
    void EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode) override;
    void EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode) override;
    void EndVisit(Cm::Ast::AddNode& addNode) override;
    void EndVisit(Cm::Ast::SubNode& subNode) override;
    void EndVisit(Cm::Ast::MulNode& mulNode) override;
    void EndVisit(Cm::Ast::DivNode& divNode) override;
    void EndVisit(Cm::Ast::RemNode& remNode) override;
    void BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode) override;
    void BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode) override;
    void EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) override;
    void EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode) override;
    void EndVisit(Cm::Ast::NotNode& notNode) override;
    void EndVisit(Cm::Ast::ComplementNode& complementNode) override;
    void Visit(Cm::Ast::AddrOfNode& addrOfNode) override;
    void Visit(Cm::Ast::DerefNode& derefNode) override;
    void Visit(Cm::Ast::PostfixIncNode& postfixIncNode) override;
    void Visit(Cm::Ast::PostfixDecNode& postfixDecNode) override;
    void BeginVisit(Cm::Ast::DotNode& dotNode) override;
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
    void Visit(Cm::Ast::ArrowNode& arrowNode) override;
    void BeginVisit(Cm::Ast::InvokeNode& invokeNode) override;
    void Visit(Cm::Ast::IndexNode& indexNode) override;

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) override;
    void Visit(Cm::Ast::CastNode& castNode) override;
    void Visit(Cm::Ast::ConstructNode& constructNode) override;
    void Visit(Cm::Ast::NewNode& newNode) override;
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void Visit(Cm::Ast::ThisNode& thisNode) override;
    void Visit(Cm::Ast::BaseNode& baseNode) override;
    void Visit(Cm::Ast::TypeNameNode& typeNameNode) override;
private:
    Cm::Sym::ValueType targetType;
    bool cast;
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* currentContainerScope;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes;
    Cm::Core::ClassTemplateRepository& classTemplateRepository;
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    Cm::Sym::EvaluationStack stack;
    bool interrupted;
    Cm::Sym::SymbolTypeSetId lookupId;
    Cm::Sym::LookupIdStack lookupIdStack;
    void EvaluateSymbol(Cm::Sym::Symbol* symbol);
};

BooleanEvaluator::BooleanEvaluator(bool cast_, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) :
    Visitor(true, true), targetType(Cm::Sym::ValueType::boolValue), cast(cast_), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScopes(fileScopes_), 
    classTemplateRepository(classTemplateRepository_), boundCompileUnit(boundCompileUnit_), interrupted(false), lookupId(Cm::Sym::SymbolTypeSetId::lookupConstantAndEnumConstantSymbols)
{
}

bool BooleanEvaluator::DoEvaluate(Cm::Ast::Node* value)
{
    try
    {
        value->Accept(*this);
        if (interrupted) return false;
        std::unique_ptr<Cm::Sym::Value> result(stack.Pop());
        if (result->GetValueType() == Cm::Sym::ValueType::boolValue)
        {
            Cm::Sym::BoolValue* boolValue = static_cast<Cm::Sym::BoolValue*>(result.get());
            return boolValue->Value() == true;
        }
        return false;
    }
    catch (...)
    {
        return false;
    }
}

void BooleanEvaluator::Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode)
{
    stack.Push(new Cm::Sym::BoolValue(booleanLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    stack.Push(new Cm::Sym::SByteValue(sbyteLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    stack.Push(new Cm::Sym::ByteValue(byteLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    stack.Push(new Cm::Sym::ShortValue(shortLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    stack.Push(new Cm::Sym::UShortValue(ushortLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    stack.Push(new Cm::Sym::IntValue(intLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    stack.Push(new Cm::Sym::UIntValue(uintLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    stack.Push(new Cm::Sym::LongValue(longLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    stack.Push(new Cm::Sym::ULongValue(ulongLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    stack.Push(new Cm::Sym::FloatValue(floatLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    stack.Push(new Cm::Sym::DoubleValue(doubleLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    stack.Push(new Cm::Sym::CharValue(charLiteralNode.Value()));
}

void BooleanEvaluator::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::WStringLiteralNode& wstringLiteralNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::UStringLiteralNode& ustringLiteralNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    interrupted = true;
}

void BooleanEvaluator::BeginVisit(Cm::Ast::EquivalenceNode& equivalenceNode)
{
    interrupted = true;
}

void BooleanEvaluator::BeginVisit(Cm::Ast::ImplicationNode& implicationNode)
{
    interrupted = true;
}

void BooleanEvaluator::EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, disjunction, cast, disjunctionNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, conjunction, cast, conjunctionNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitOrNode& bitOrNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitOr, cast, bitOrNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitXorNode& bitXorNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitXor, cast, bitXorNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitAndNode& bitAndNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitAnd, cast, bitAndNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::EqualNode& equalNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, equal, cast, equalNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::NotEqualNode& notEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, notEqual, cast, notEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::LessNode& lessNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, less, cast, lessNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::GreaterNode& greaterNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greater, cast, greaterNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, lessOrEqual, cast, lessOrEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greaterOrEqual, cast, greaterOrEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftLeft, cast, shiftLeftNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftRight, cast, shiftRightNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::AddNode& addNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, add, cast, addNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::SubNode& subNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, sub, cast, subNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::MulNode& mulNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, mul, cast, mulNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::DivNode& divNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, div, cast, divNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::RemNode& remNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, rem, cast, remNode.GetSpan());
}

void BooleanEvaluator::BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode)
{
    interrupted = true;
}

void BooleanEvaluator::BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode)
{
    interrupted = true;
}

void BooleanEvaluator::EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, unaryPlus, cast, unaryPlusNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, negate, cast, unaryMinusNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::NotNode& notNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, not_, cast, notNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ComplementNode& complementNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, complement, cast, complementNode.GetSpan());
}

void BooleanEvaluator::Visit(Cm::Ast::AddrOfNode& addrOfNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::DerefNode& derefNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::PostfixIncNode& postfixIncNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::PostfixDecNode& postfiDecNode)
{
    interrupted = true;
}

void BooleanEvaluator::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&classNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void BooleanEvaluator::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&namespaceNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void BooleanEvaluator::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&enumTypeNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void BooleanEvaluator::BeginVisit(Cm::Ast::DotNode& dotNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupContainerSymbols;
}

void BooleanEvaluator::EndVisit(Cm::Ast::DotNode& dotNode)
{
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsScopedValue())
    {
        ScopedValue* scopedValue = static_cast<ScopedValue*>(value.get());
        Cm::Sym::ContainerSymbol* containerSymbol = scopedValue->ContainerSymbol();
        Cm::Sym::ContainerScope* scope = containerSymbol->GetContainerScope();
        Cm::Sym::Symbol* symbol = scope->Lookup(dotNode.MemberStr(), lookupId);
        if (symbol)
        {
            EvaluateSymbol(symbol);
        }
        else
        {
            interrupted = true;
        }
    }
    else
    {
        interrupted = true;
    }
}

void BooleanEvaluator::Visit(Cm::Ast::ArrowNode& arrowNode)
{
    interrupted = true;
}

void BooleanEvaluator::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::IndexNode& indexNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::SizeOfNode& sizeOfNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::CastNode& castNode)
{
    Cm::Ast::Node* targetTypeExpr = castNode.TargetTypeExpr();
    std::unique_ptr<Cm::Sym::TypeSymbol> type(ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, targetTypeExpr));
    Cm::Sym::SymbolType symbolType = type->GetSymbolType();
    Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType, false);
    stack.Push(IsAlwaysTrue(castNode.SourceExpr(), symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit) ? new Cm::Sym::BoolValue(true) : new Cm::Sym::BoolValue(false));
}

void BooleanEvaluator::Visit(Cm::Ast::ConstructNode& constructNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::NewNode& newNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    interrupted = true;
}

void BooleanEvaluator::EvaluateSymbol(Cm::Sym::Symbol* symbol)
{
    if (symbol->IsContainerSymbol())
    {
        Cm::Sym::ContainerSymbol* containerSymbol = static_cast<Cm::Sym::ContainerSymbol*>(symbol);
        stack.Push(new ScopedValue(containerSymbol));
    }
    else if (symbol->IsConstantSymbol())
    {
        Cm::Sym::ConstantSymbol* constantSymbol = static_cast<Cm::Sym::ConstantSymbol*>(symbol);
        if (!constantSymbol->GetValue())
        {
            Cm::Ast::Node* node = symbolTable.GetNode(constantSymbol);
            if (node->IsConstantNode())
            {
                Cm::Ast::ConstantNode* constantNode = static_cast<Cm::Ast::ConstantNode*>(node);
                BindConstant(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, constantNode);
            }
            else
            {
                throw std::runtime_error("node is not constant node");
            }
        }
        stack.Push(constantSymbol->GetValue()->Clone());
    }
    else if (symbol->IsEnumConstantSymbol())
    {
        Cm::Sym::EnumConstantSymbol* enumConstantSymbol = static_cast<Cm::Sym::EnumConstantSymbol*>(symbol);
        if (!enumConstantSymbol->GetValue())
        {
            Cm::Ast::Node* node = symbolTable.GetNode(enumConstantSymbol);
            if (node->IsEnumConstantNode())
            {
                Cm::Ast::EnumConstantNode* enumConstantNode = static_cast<Cm::Ast::EnumConstantNode*>(node);
                Cm::Sym::ContainerScope* enumConstantContainerScope = symbolTable.GetContainerScope(enumConstantNode);
                BindEnumConstant(symbolTable, enumConstantContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, enumConstantNode);
            }
            else
            {
                throw std::runtime_error("node is not enumeration constant node");
            }
        }
        stack.Push(enumConstantSymbol->GetValue()->Clone());
    }
    else
    {
        interrupted = true;
    }
}

void BooleanEvaluator::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    Cm::Sym::Symbol* symbol = currentContainerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, lookupId);
    if (!symbol)
    {
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
        {
            symbol = fileScope->Lookup(identifierNode.Str(), lookupId);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        EvaluateSymbol(symbol);
    }
    else
    {
        interrupted = true;
    }
}

void BooleanEvaluator::Visit(Cm::Ast::ThisNode& thisNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::BaseNode& baseNode)
{
    interrupted = true;
}

void BooleanEvaluator::Visit(Cm::Ast::TypeNameNode& typeNameNode)
{
    interrupted = true;
}

bool IsAlwaysTrue(Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes,
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    try
    {
        BooleanEvaluator evaluator(false, symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit);
        return evaluator.DoEvaluate(value);
    }
    catch (...)
    {
    }
    return false;
}

} } // namespace Cm::Bind
