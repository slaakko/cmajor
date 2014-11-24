namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Cm::Ast::Node*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar IdentifierGrammar
    {
        Identifier: Cm::Ast::Node*;
        QualifiedId: Cm::Ast::Node*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var std::unique_ptr<DerivedTypeExprNode> node): Cm::Ast::Node*;
        PrefixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PrimaryTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
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
}
