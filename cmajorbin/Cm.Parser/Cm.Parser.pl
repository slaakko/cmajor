namespace Cm.Parser
{
    grammar BasicTypeGrammar
    {
        BasicType: Cm::Ast::Node*;
    }
    grammar ConstantGrammar
    {
        Constant(ParsingContext* ctx): Cm::Ast::Node*;
    }
    grammar ParameterGrammar
    {
        ParameterList(ParsingContext* ctx, Cm::Ast::Node* owner);
        Parameter(ParsingContext* ctx, Cm::Ast::Node* owner);
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
    grammar DelegateGrammar
    {
        Delegate(ParsingContext* ctx): Cm::Ast::Node*;
        ClassDelegate(ParsingContext* ctx): Cm::Ast::Node*;
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
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        LabelId: std::string;
        Label(var std::string label): Cm::Ast::LabelNode*;
        TargetLabel(var std::string label): Cm::Ast::LabelNode*;
        LabeledStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        SimpleStatement(ParsingContext* ctx, var std::unique_ptr<Node> expr): Cm::Ast::StatementNode*;
        ControlStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ReturnStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ConditionalStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        SwitchStatement(ParsingContext* ctx): Cm::Ast::SwitchStatementNode*;
        CaseStatement(ParsingContext* ctx, var std::unique_ptr<CaseStatementNode> caseS): Cm::Ast::CaseStatementNode*;
        DefaultStatement(ParsingContext* ctx): Cm::Ast::DefaultStatementNode*;
        CaseList(ParsingContext* ctx, Cm::Ast::CaseStatementNode* caseS);
        GotoCaseStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        GotoDefaultStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        WhileStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DoStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        RangeForStatement(ParsingContext* ctx, var std::unique_ptr<Node> varType, var std::unique_ptr<IdentifierNode> varId): Cm::Ast::StatementNode*;
        ForStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ForInitStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        CompoundStatement(ParsingContext* ctx): Cm::Ast::CompoundStatementNode*;
        BreakStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ContinueStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        GotoStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        TypedefStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        AssignmentStatement(ParsingContext* ctx, var std::unique_ptr<Node> targetExpr): Cm::Ast::StatementNode*;
        ConstructionStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DeleteStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        DestroyStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ThrowStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        TryStatement(ParsingContext* ctx): Cm::Ast::TryStatementNode*;
        Handlers(ParsingContext* ctx, Cm::Ast::TryStatementNode* tryS);
        Handler(ParsingContext* ctx): Cm::Ast::CatchNode*;
        AssertStatement(ParsingContext* ctx): Cm::Ast::StatementNode*;
        ConditionalCompilationStatement(ParsingContext* ctx): Cm::Ast::CondCompStatementNode*;
        ConditionalCompilationExpr: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationDisjunction(var Span s): Cm::Ast::CondCompExprNode*;
        ConditionalCompilationConjunction(var Span s): Cm::Ast::CondCompExprNode*;
        ConditionalCompilationPrefix: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationPrimary: Cm::Ast::CondCompExprNode*;
        ConditionalCompilationSymbol: Cm::Ast::CondCompSymbolNode*;
        Symbol: std::string;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var std::unique_ptr<TemplateIdNode> templateId): Cm::Ast::Node*;
    }
    grammar TypedefGrammar
    {
        Typedef(ParsingContext* ctx): Cm::Ast::Node*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var std::unique_ptr<DerivedTypeExprNode> node): Cm::Ast::Node*;
        PrefixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PostfixTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
        PrimaryTypeExpr(ParsingContext* ctx, Cm::Ast::DerivedTypeExprNode* node);
    }
}
