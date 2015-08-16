namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Node*;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx): Node*;
        Equivalence(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Implication(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Disjunction(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Conjunction(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitOr(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitXor(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        BitAnd(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Equality(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Relational(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Shift(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Additive(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Multiplicative(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s, var Operator op): Node*;
        Prefix(ParsingContext* ctx, var Span s, var Operator op): Node*;
        Postfix(ParsingContext* ctx, var UniquePtr<Node> expr, var Span s): Node*;
        Primary(ParsingContext* ctx): Node*;
        SizeOfExpr(ParsingContext* ctx): Node*;
        CastExpr(ParsingContext* ctx): Node*;
        ConstructExpr(ParsingContext* ctx): Node*;
        NewExpr(ParsingContext* ctx): Node*;
        ArgumentList(ParsingContext* ctx, Node* node);
        ExpressionList(ParsingContext* ctx, Node* node);
    }
    grammar IdentifierGrammar
    {
        Identifier: IdentifierNode*;
        QualifiedId: IdentifierNode*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar LiteralGrammar
    {
        Literal: Node*;
        BooleanLiteral: Node*;
        IntegerLiteral(var int start): Node*;
        FloatingLiteral(var int start): Node*;
        CharLiteral: Node*;
        StringLiteral(var string r): Node*;
        NullLiteral: Node*;
    }
    grammar SpecifierGrammar
    {
        Specifiers: Specifiers;
        Specifier: Specifiers;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var UniquePtr<TemplateIdNode> templateId): Node*;
        TemplateParameter(ParsingContext* ctx): TemplateParameterNode*;
        TemplateParameterList(ParsingContext* ctx, Node* owner);
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var UniquePtr<DerivedTypeExprNode> node): Node*;
        PrefixTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node, var Span s);
        PrimaryTypeExpr(ParsingContext* ctx, DerivedTypeExprNode* node);
    }
}
