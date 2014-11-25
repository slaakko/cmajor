namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Cm::Ast::Node*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var std::unique_ptr<DerivedTypeExprNode> node): Cm::Ast::Node*;
        PrefixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PrimaryTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
    }
    grammar EnumerationGrammar
    {
        EnumType(ParsingContext* ctx): Cm::Ast::EnumTypeNode*;
        EnumConstants(ParsingContext* ctx, Cm::Ast::EnumTypeNode* enumType);
        EnumConstant(ParsingContext* ctx, Cm::Ast::EnumTypeNode* enumType, var Span s): Cm::Ast::Node*;
    }
    grammar LiteralGrammar
    {
        Literal: Cm::Ast::Node*;
        BooleanLiteral: Cm::Ast::Node*;
        IntegerLiteral(var int start): Cm::Ast::Node*;
        FloatingLiteral(var int start): Cm::Ast::Node*;
        CharLiteral: Cm::Ast::Node*;
        StringLiteral(var std::string r): Cm::Ast::Node*;
        NullLiteral: Cm::Ast::Node*;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var std::unique_ptr<TemplateIdNode> templateId): Cm::Ast::Node*;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx): Cm::Ast::Node*;
        Equivalence(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Implication(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Disjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Conjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitOr(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitXor(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        BitAnd(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Equality(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Relational(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Shift(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Additive(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Multiplicative(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Cm::Ast::Node*;
        Prefix(ParsingContext* ctx, var Span s, var Operator op): Cm::Ast::Node*;
        Postfix(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Cm::Ast::Node*;
        Primary(ParsingContext* ctx): Cm::Ast::Node*;
        SizeOfExpr(ParsingContext* ctx): Cm::Ast::Node*;
        CastExpr(ParsingContext* ctx): Cm::Ast::Node*;
        ConstructExpr(ParsingContext* ctx): Cm::Ast::Node*;
        NewExpr(ParsingContext* ctx): Cm::Ast::Node*;
        ArgumentList(ParsingContext* ctx, Cm::Ast::Node* node);
        ExpressionList(ParsingContext* ctx, Cm::Ast::Node* node);
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar IdentifierGrammar
    {
        Identifier: Cm::Ast::IdentifierNode*;
        QualifiedId: Cm::Ast::IdentifierNode*;
    }
    grammar SpecifierGrammar
    {
        Specifiers: Cm::Ast::Specifiers;
        Specifier: Cm::Ast::Specifiers;
    }
}
