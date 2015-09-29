/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Constant.hpp>
#include <Cm.Bind/Enumeration.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Expression.hpp>
#include <functional>
#include <utility>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

typedef Cm::Sym::Value* (*BinaryOperatorFun)(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span);
typedef Cm::Sym::Value* (*UnaryOperatorFun)(Cm::Sym::Value* subject, const Span& span);

class EvaluationStack
{
public:
    void Push(Cm::Sym::Value* value);
    Cm::Sym::Value* Pop();
private:
    std::stack<std::unique_ptr<Cm::Sym::Value>> stack;
};

void EvaluationStack::Push(Cm::Sym::Value* value)
{ 
    stack.push(std::unique_ptr<Cm::Sym::Value>(value)); 
}

Cm::Sym::Value* EvaluationStack::Pop() 
{ 
    if (stack.empty()) 
    {
        throw std::runtime_error("evaluation stack is empty");
    }
    std::unique_ptr<Cm::Sym::Value> value = std::move(stack.top()); 
    stack.pop(); 
    return value.release(); 
}

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
    NotSupported, Not<Cm::Sym::BoolValue>, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
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
    NotSupported, NotSupported, NotSupported, 
    UnaryPlus<Cm::Sym::SByteValue>, UnaryPlus<Cm::Sym::ByteValue>, UnaryPlus<Cm::Sym::ShortValue>, UnaryPlus<Cm::Sym::UShortValue>, UnaryPlus<Cm::Sym::IntValue>, UnaryPlus<Cm::Sym::UIntValue>, 
    UnaryPlus<Cm::Sym::LongValue>, UnaryPlus<Cm::Sym::ULongValue>, UnaryPlus<Cm::Sym::FloatValue>, UnaryPlus<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Negate(Cm::Sym::Value* subject, const Span& span)
{
    return UnaryEvaluate<ValueT>(subject, std::negate<typename ValueT::OperandType>());
}

UnaryOperatorFun negate[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Negate<Cm::Sym::SByteValue>, Negate<Cm::Sym::ByteValue>, Negate<Cm::Sym::ShortValue>, Negate<Cm::Sym::UShortValue>, Negate<Cm::Sym::IntValue>, Negate<Cm::Sym::UIntValue>, 
    Negate<Cm::Sym::LongValue>, Negate<Cm::Sym::ULongValue>, Negate<Cm::Sym::FloatValue>, Negate<Cm::Sym::DoubleValue>
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
    NotSupported, NotSupported, NotSupported, 
    Complement<Cm::Sym::SByteValue>, Complement<Cm::Sym::ByteValue>, Complement<Cm::Sym::ShortValue>, Complement<Cm::Sym::UShortValue>, Complement<Cm::Sym::IntValue>, Complement<Cm::Sym::UIntValue>, 
    Complement<Cm::Sym::LongValue>, Complement<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

void EvaluateUnaryOp(Cm::Sym::ValueType targetType, EvaluationStack& stack, UnaryOperatorFun* fun, const Span& span)
{
    std::unique_ptr<Cm::Sym::Value> subject(stack.Pop());
    Cm::Sym::ValueType subjectType = subject->GetValueType();
    Cm::Sym::ValueType operationType = subjectType;
    if (targetType > subjectType)
    {
        operationType = targetType;
    }
    std::unique_ptr<Cm::Sym::Value> subject_(subject->As(operationType, false, span));
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
    NotSupported, NotSupported, NotSupported, 
    ShiftLeft<Cm::Sym::SByteValue>, ShiftLeft<Cm::Sym::ByteValue>, ShiftLeft<Cm::Sym::ShortValue>, ShiftLeft<Cm::Sym::UShortValue>, ShiftLeft<Cm::Sym::IntValue>, ShiftLeft<Cm::Sym::UIntValue>, 
    ShiftLeft<Cm::Sym::LongValue>, ShiftLeft<Cm::Sym::ULongValue>, NotSupported, NotSupported
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
    NotSupported, NotSupported, NotSupported, 
    ShiftRight<Cm::Sym::SByteValue>, ShiftRight<Cm::Sym::ByteValue>, ShiftRight<Cm::Sym::ShortValue>, ShiftRight<Cm::Sym::UShortValue>, ShiftRight<Cm::Sym::IntValue>, ShiftRight<Cm::Sym::UIntValue>, 
    ShiftRight<Cm::Sym::LongValue>, ShiftRight<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Add(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::plus<typename ValueT::OperandType>());
}

BinaryOperatorFun add[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Add<Cm::Sym::SByteValue>, Add<Cm::Sym::ByteValue>, Add<Cm::Sym::ShortValue>, Add<Cm::Sym::UShortValue>, Add<Cm::Sym::IntValue>, Add<Cm::Sym::UIntValue>, 
    Add<Cm::Sym::LongValue>, Add<Cm::Sym::ULongValue>, Add<Cm::Sym::FloatValue>, Add<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Sub(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::minus<typename ValueT::OperandType>());
}

BinaryOperatorFun sub[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Sub<Cm::Sym::SByteValue>, Sub<Cm::Sym::ByteValue>, Sub<Cm::Sym::ShortValue>, Sub<Cm::Sym::UShortValue>, Sub<Cm::Sym::IntValue>, Sub<Cm::Sym::UIntValue>, 
    Sub<Cm::Sym::LongValue>, Sub<Cm::Sym::ULongValue>, Sub<Cm::Sym::FloatValue>, Sub<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Mul(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::multiplies<typename ValueT::OperandType>());
}

BinaryOperatorFun mul[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Mul<Cm::Sym::SByteValue>, Mul<Cm::Sym::ByteValue>, Mul<Cm::Sym::ShortValue>, Mul<Cm::Sym::UShortValue>, Mul<Cm::Sym::IntValue>, Mul<Cm::Sym::UIntValue>, 
    Mul<Cm::Sym::LongValue>, Mul<Cm::Sym::ULongValue>, Mul<Cm::Sym::FloatValue>, Mul<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Div(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::divides<typename ValueT::OperandType>());
}

BinaryOperatorFun div[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Div<Cm::Sym::SByteValue>, Div<Cm::Sym::ByteValue>, Div<Cm::Sym::ShortValue>, Div<Cm::Sym::UShortValue>, Div<Cm::Sym::IntValue>, Div<Cm::Sym::UIntValue>, 
    Div<Cm::Sym::LongValue>, Div<Cm::Sym::ULongValue>, Div<Cm::Sym::FloatValue>, Div<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Rem(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::modulus<typename ValueT::OperandType>());
}

BinaryOperatorFun rem[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    Rem<Cm::Sym::SByteValue>, Rem<Cm::Sym::ByteValue>, Rem<Cm::Sym::ShortValue>, Rem<Cm::Sym::UShortValue>, Rem<Cm::Sym::IntValue>, Rem<Cm::Sym::UIntValue>, 
    Rem<Cm::Sym::LongValue>, Rem<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitAnd(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_and<typename ValueT::OperandType>());
}

BinaryOperatorFun bitAnd[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    BitAnd<Cm::Sym::SByteValue>, BitAnd<Cm::Sym::ByteValue>, BitAnd<Cm::Sym::ShortValue>, BitAnd<Cm::Sym::UShortValue>, BitAnd<Cm::Sym::IntValue>, BitAnd<Cm::Sym::UIntValue>, 
    BitAnd<Cm::Sym::LongValue>, BitAnd<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitOr(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_or<typename ValueT::OperandType>());
}

BinaryOperatorFun bitOr[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    BitOr<Cm::Sym::SByteValue>, BitOr<Cm::Sym::ByteValue>, BitOr<Cm::Sym::ShortValue>, BitOr<Cm::Sym::UShortValue>, BitOr<Cm::Sym::IntValue>, BitOr<Cm::Sym::UIntValue>, 
    BitOr<Cm::Sym::LongValue>, BitOr<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* BitXor(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::bit_xor<typename ValueT::OperandType>());
}

BinaryOperatorFun bitXor[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotSupported, NotSupported, 
    BitXor<Cm::Sym::SByteValue>, BitXor<Cm::Sym::ByteValue>, BitXor<Cm::Sym::ShortValue>, BitXor<Cm::Sym::UShortValue>, BitXor<Cm::Sym::IntValue>, BitXor<Cm::Sym::UIntValue>, 
    BitXor<Cm::Sym::LongValue>, BitXor<Cm::Sym::ULongValue>, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Disjunction(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_or<typename ValueT::OperandType>());
}

BinaryOperatorFun disjunction[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Disjunction<Cm::Sym::BoolValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

template<typename ValueT>
Cm::Sym::Value* Conjunction(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryEvaluate<ValueT>(left, right, std::logical_and<typename ValueT::OperandType>());
}

BinaryOperatorFun conjunction[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Disjunction<Cm::Sym::BoolValue>, 
    NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported, NotSupported
};

void EvaluateBinOp(Cm::Sym::ValueType targetType, EvaluationStack& stack, BinaryOperatorFun* fun, const Span& span)
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
    std::unique_ptr<Cm::Sym::Value> left_(left->As(operationType, false, span));
    std::unique_ptr<Cm::Sym::Value> right_(right->As(operationType, false, span));
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
    NotSupported, Equal<Cm::Sym::BoolValue>, Equal<Cm::Sym::CharValue>, 
    Equal<Cm::Sym::SByteValue>, Equal<Cm::Sym::ByteValue>, Equal<Cm::Sym::ShortValue>, Equal<Cm::Sym::UShortValue>, Equal<Cm::Sym::IntValue>, Equal<Cm::Sym::UIntValue>, 
    Equal<Cm::Sym::LongValue>, Equal<Cm::Sym::ULongValue>, Equal<Cm::Sym::FloatValue>, Equal<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* NotEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::not_equal_to<typename ValueT::OperandType>());
}

BinaryOperatorFun notEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, NotEqual<Cm::Sym::BoolValue>, NotEqual<Cm::Sym::CharValue>, 
    NotEqual<Cm::Sym::SByteValue>, NotEqual<Cm::Sym::ByteValue>, NotEqual<Cm::Sym::ShortValue>, NotEqual<Cm::Sym::UShortValue>, NotEqual<Cm::Sym::IntValue>, NotEqual<Cm::Sym::UIntValue>, 
    NotEqual<Cm::Sym::LongValue>, NotEqual<Cm::Sym::ULongValue>, NotEqual<Cm::Sym::FloatValue>, NotEqual<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Less(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less<typename ValueT::OperandType>());
}

BinaryOperatorFun less[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Less<Cm::Sym::BoolValue>, Less<Cm::Sym::CharValue>, 
    Less<Cm::Sym::SByteValue>, Less<Cm::Sym::ByteValue>, Less<Cm::Sym::ShortValue>, Less<Cm::Sym::UShortValue>, Less<Cm::Sym::IntValue>, Less<Cm::Sym::UIntValue>, 
    Less<Cm::Sym::LongValue>, Less<Cm::Sym::ULongValue>, Less<Cm::Sym::FloatValue>, Less<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* Greater(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater<typename ValueT::OperandType>());
}

BinaryOperatorFun greater[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, Greater<Cm::Sym::BoolValue>, Greater<Cm::Sym::CharValue>, 
    Greater<Cm::Sym::SByteValue>, Greater<Cm::Sym::ByteValue>, Greater<Cm::Sym::ShortValue>, Greater<Cm::Sym::UShortValue>, Greater<Cm::Sym::IntValue>, Greater<Cm::Sym::UIntValue>, 
    Greater<Cm::Sym::LongValue>, Greater<Cm::Sym::ULongValue>, Greater<Cm::Sym::FloatValue>, Greater<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* LessOrEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::less_equal<typename ValueT::OperandType>());
}

BinaryOperatorFun lessOrEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, LessOrEqual<Cm::Sym::BoolValue>, LessOrEqual<Cm::Sym::CharValue>, 
    LessOrEqual<Cm::Sym::SByteValue>, LessOrEqual<Cm::Sym::ByteValue>, LessOrEqual<Cm::Sym::ShortValue>, LessOrEqual<Cm::Sym::UShortValue>, LessOrEqual<Cm::Sym::IntValue>, LessOrEqual<Cm::Sym::UIntValue>, 
    LessOrEqual<Cm::Sym::LongValue>, LessOrEqual<Cm::Sym::ULongValue>, LessOrEqual<Cm::Sym::FloatValue>, LessOrEqual<Cm::Sym::DoubleValue>
};

template<typename ValueT>
Cm::Sym::Value* GreaterOrEqual(Cm::Sym::Value* left, Cm::Sym::Value* right, const Span& span)
{
    return BinaryPredEvaluate<ValueT>(left, right, std::greater_equal<typename ValueT::OperandType>());
}

BinaryOperatorFun greaterOrEqual[uint8_t(Cm::Sym::ValueType::max)] =
{
    NotSupported, GreaterOrEqual<Cm::Sym::BoolValue>, GreaterOrEqual<Cm::Sym::CharValue>, 
    GreaterOrEqual<Cm::Sym::SByteValue>, GreaterOrEqual<Cm::Sym::ByteValue>, GreaterOrEqual<Cm::Sym::ShortValue>, GreaterOrEqual<Cm::Sym::UShortValue>, GreaterOrEqual<Cm::Sym::IntValue>, GreaterOrEqual<Cm::Sym::UIntValue>, 
    GreaterOrEqual<Cm::Sym::LongValue>, GreaterOrEqual<Cm::Sym::ULongValue>, GreaterOrEqual<Cm::Sym::FloatValue>, GreaterOrEqual<Cm::Sym::DoubleValue>
};

class Evaluator : public Cm::Ast::Visitor
{
public:
    Evaluator(Cm::Sym::ValueType targetType_, bool cast, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, 
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::Core::ClassTemplateRepository& classTemplateRepository_);
    Cm::Sym::Value* DoEvaluate(Cm::Ast::Node* value);
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
    EvaluationStack stack;
    Cm::Sym::SymbolTypeSetId lookupId;
    Cm::Sym::LookupIdStack lookupIdStack;
    void EvaluateSymbol(Cm::Sym::Symbol* symbol);
};

Evaluator::Evaluator(Cm::Sym::ValueType targetType_, bool cast_, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, Cm::Core::ClassTemplateRepository& classTemplateRepository_) :
    Visitor(true, true), targetType(targetType_), cast(cast_), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScopes(fileScopes_), 
    classTemplateRepository(classTemplateRepository_), lookupId(Cm::Sym::SymbolTypeSetId::lookupConstantAndEnumConstantSymbols)
{
}

Cm::Sym::Value* Evaluator::DoEvaluate(Cm::Ast::Node* value)
{
    value->Accept(*this);
    std::unique_ptr<Cm::Sym::Value> result(stack.Pop());
    result.reset(result->As(targetType, cast, value->GetSpan()));
    return result.release();
}

void Evaluator::Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode)
{
    stack.Push(new Cm::Sym::BoolValue(booleanLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    stack.Push(new Cm::Sym::SByteValue(sbyteLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    stack.Push(new Cm::Sym::ByteValue(byteLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    stack.Push(new Cm::Sym::ShortValue(shortLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    stack.Push(new Cm::Sym::UShortValue(ushortLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    stack.Push(new Cm::Sym::IntValue(intLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    stack.Push(new Cm::Sym::UIntValue(uintLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    stack.Push(new Cm::Sym::LongValue(longLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    stack.Push(new Cm::Sym::ULongValue(ulongLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    stack.Push(new Cm::Sym::FloatValue(floatLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    stack.Push(new Cm::Sym::DoubleValue(doubleLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    stack.Push(new Cm::Sym::CharValue(charLiteralNode.Value()));
}

void Evaluator::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", stringLiteralNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", nullLiteralNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::EquivalenceNode& equivalenceNode)
{
    throw Cm::Core::Exception("operation not supported", equivalenceNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::ImplicationNode& implicationNode)
{
    throw Cm::Core::Exception("operation not supported", implicationNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode)
{
    EvaluateBinOp(targetType, stack, disjunction, disjunctionNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    EvaluateBinOp(targetType, stack, conjunction, conjunctionNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitOrNode& bitOrNode)
{
    EvaluateBinOp(targetType, stack, bitOr, bitOrNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitXorNode& bitXorNode)
{
    EvaluateBinOp(targetType, stack, bitXor, bitXorNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::BitAndNode& bitAndNode)
{
    EvaluateBinOp(targetType, stack, bitAnd, bitAndNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::EqualNode& equalNode)
{
    EvaluateBinOp(targetType, stack, equal, equalNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::NotEqualNode& notEqualNode)
{
    EvaluateBinOp(targetType, stack, notEqual, notEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::LessNode& lessNode)
{
    EvaluateBinOp(targetType, stack, less, lessNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::GreaterNode& greaterNode)
{
    EvaluateBinOp(targetType, stack, greater, greaterNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode)
{
    EvaluateBinOp(targetType, stack, lessOrEqual, lessOrEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode)
{
    EvaluateBinOp(targetType, stack, greaterOrEqual, greaterOrEqualNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode)
{
    EvaluateBinOp(targetType, stack, shiftLeft, shiftLeftNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode)
{
    EvaluateBinOp(targetType, stack, shiftRight, shiftRightNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::AddNode& addNode)
{
    EvaluateBinOp(targetType, stack, add, addNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::SubNode& subNode)
{
    EvaluateBinOp(targetType, stack, sub, subNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::MulNode& mulNode)
{
    EvaluateBinOp(targetType, stack, mul, mulNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::DivNode& divNode)
{
    EvaluateBinOp(targetType, stack, div, divNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::RemNode& remNode)
{
    EvaluateBinOp(targetType, stack, rem, remNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", prefixIncNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", prefixDecNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode)
{
    EvaluateUnaryOp(targetType, stack, unaryPlus, unaryPlusNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    EvaluateUnaryOp(targetType, stack, negate, unaryMinusNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::NotNode& notNode)
{
    EvaluateUnaryOp(targetType, stack, not_, notNode.GetSpan());
}

void Evaluator::EndVisit(Cm::Ast::ComplementNode& complementNode)
{
    EvaluateUnaryOp(targetType, stack, complement, complementNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::AddrOfNode& addrOfNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", addrOfNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::DerefNode& derefNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", derefNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::PostfixIncNode& postfixIncNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", postfixIncNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::PostfixDecNode& postfiDecNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", postfiDecNode.GetSpan());
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
    virtual Ir::Intf::Object* CreateIrObject() const override { throw std::runtime_error("member function not applicable"); }
private:
    Cm::Sym::ContainerSymbol* containerSymbol;
};

void Evaluator::BeginVisit(Cm::Ast::ClassNode& classNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&classNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void Evaluator::BeginVisit(Cm::Ast::NamespaceNode& namespaceNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&namespaceNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void Evaluator::BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&enumTypeNode);
    stack.Push(new ScopedValue(scope->Container()));
}

void Evaluator::BeginVisit(Cm::Ast::DotNode& dotNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupContainerSymbols;
}

void Evaluator::EndVisit(Cm::Ast::DotNode& dotNode)
{
    lookupId = lookupIdStack.Pop();
    std::unique_ptr<Cm::Sym::Value> value(stack.Pop());
    if (value->IsScopedValue())
    {
        ScopedValue* scopedValue = static_cast<ScopedValue*>(value.get());
        Cm::Sym::ContainerSymbol* containerSymbol = scopedValue->ContainerSymbol();
        Cm::Sym::ContainerScope* scope = containerSymbol->GetContainerScope();
        Cm::Sym::Symbol* symbol = scope->Lookup(dotNode.MemberId()->Str(), lookupId);
        if (symbol)
        {
            EvaluateSymbol(symbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + containerSymbol->FullName() + "' does not have member '" + dotNode.MemberId()->Str() + "'", dotNode.GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("expression '" + dotNode.Subject()->FullName() + "' must denote a namespace, class type or enumerated type", dotNode.Subject()->GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::ArrowNode& arrowNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", arrowNode.GetSpan());
}

void Evaluator::BeginVisit(Cm::Ast::InvokeNode& invokeNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", invokeNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::IndexNode& indexNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", indexNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::SizeOfNode& sizeOfNode)
{
    throw Cm::Core::Exception("not implemented yet", sizeOfNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::CastNode& castNode)
{
    Cm::Ast::Node* targetTypeExpr = castNode.TargetTypeExpr();
    std::unique_ptr<Cm::Sym::TypeSymbol> type(ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, targetTypeExpr));
    Cm::Sym::SymbolType symbolType = type->GetSymbolType();
    Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType);
    stack.Push(Evaluate(valueType, true, castNode.SourceExpr(), symbolTable, currentContainerScope, fileScopes, classTemplateRepository));
}

void Evaluator::Visit(Cm::Ast::ConstructNode& constructNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", constructNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::NewNode& newNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", newNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", templateIdNode.GetSpan());
}

void Evaluator::EvaluateSymbol(Cm::Sym::Symbol* symbol)
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
                BindConstant(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, constantNode);
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
                BindEnumConstant(symbolTable, enumConstantContainerScope, fileScopes, classTemplateRepository, enumConstantNode);
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
        throw Cm::Core::Exception("cannot evaluate statically", symbol->GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::IdentifierNode& identifierNode)
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
        throw Cm::Core::Exception("constant or enumeration constant symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
}

void Evaluator::Visit(Cm::Ast::ThisNode& thisNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", thisNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::BaseNode& baseNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", baseNode.GetSpan());
}

void Evaluator::Visit(Cm::Ast::TypeNameNode& typeNameNode)
{
    throw Cm::Core::Exception("cannot evaluate statically", typeNameNode.GetSpan());
}

Cm::Sym::Value* Evaluate(Cm::Sym::ValueType targetType, bool cast, Cm::Ast::Node* value, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, 
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, Cm::Core::ClassTemplateRepository& classTemplateRepository)
{
    Evaluator evaluator(targetType, cast, symbolTable, currentContainerScope, fileScopes, classTemplateRepository);
    return evaluator.DoEvaluate(value);
}

class BooleanEvaluator : public Cm::Ast::Visitor
{
public:
    BooleanEvaluator(bool cast, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::Core::ClassTemplateRepository& classTemplateRepository_);
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
    EvaluationStack stack;
    bool interrupted;
    Cm::Sym::SymbolTypeSetId lookupId;
    Cm::Sym::LookupIdStack lookupIdStack;
    void EvaluateSymbol(Cm::Sym::Symbol* symbol);
};

BooleanEvaluator::BooleanEvaluator(bool cast_, Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
    Cm::Core::ClassTemplateRepository& classTemplateRepository_) :
    Visitor(true, true), targetType(Cm::Sym::ValueType::boolValue), cast(cast_), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScopes(fileScopes_), 
    classTemplateRepository(classTemplateRepository_), interrupted(false), lookupId(Cm::Sym::SymbolTypeSetId::lookupConstantAndEnumConstantSymbols)
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
    EvaluateBinOp(targetType, stack, disjunction, disjunctionNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, conjunction, conjunctionNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitOrNode& bitOrNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitOr, bitOrNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitXorNode& bitXorNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitXor, bitXorNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::BitAndNode& bitAndNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, bitAnd, bitAndNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::EqualNode& equalNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, equal, equalNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::NotEqualNode& notEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, notEqual, notEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::LessNode& lessNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, less, lessNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::GreaterNode& greaterNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greater, greaterNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, lessOrEqual, lessOrEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, greaterOrEqual, greaterOrEqualNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftLeft, shiftLeftNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, shiftRight, shiftRightNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::AddNode& addNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, add, addNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::SubNode& subNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, sub, subNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::MulNode& mulNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, mul, mulNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::DivNode& divNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, div, divNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::RemNode& remNode)
{
    if (interrupted) return;
    EvaluateBinOp(targetType, stack, rem, remNode.GetSpan());
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
    EvaluateUnaryOp(targetType, stack, unaryPlus, unaryPlusNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, negate, unaryMinusNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::NotNode& notNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, not_, notNode.GetSpan());
}

void BooleanEvaluator::EndVisit(Cm::Ast::ComplementNode& complementNode)
{
    if (interrupted) return;
    EvaluateUnaryOp(targetType, stack, complement, complementNode.GetSpan());
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
        Cm::Sym::Symbol* symbol = scope->Lookup(dotNode.MemberId()->Str(), lookupId);
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
    std::unique_ptr<Cm::Sym::TypeSymbol> type(ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, targetTypeExpr));
    Cm::Sym::SymbolType symbolType = type->GetSymbolType();
    Cm::Sym::ValueType valueType = GetValueTypeFor(symbolType);
    stack.Push(IsAlwaysTrue(castNode.SourceExpr(), symbolTable, currentContainerScope, fileScopes, classTemplateRepository) ? new Cm::Sym::BoolValue(true) : new Cm::Sym::BoolValue(false));
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
                BindConstant(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, constantNode);
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
                BindEnumConstant(symbolTable, enumConstantContainerScope, fileScopes, classTemplateRepository, enumConstantNode);
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
    Cm::Core::ClassTemplateRepository& classTemplateRepository)
{
    try
    {
        BooleanEvaluator evaluator(false, symbolTable, currentContainerScope, fileScopes, classTemplateRepository);
        return evaluator.DoEvaluate(value);
    }
    catch (...)
    {
    }
    return false;
}

} } // namespace Cm::Bind
