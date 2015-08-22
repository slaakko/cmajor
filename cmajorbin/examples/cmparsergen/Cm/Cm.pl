namespace Cm
{
    grammar BasicTypeGrammar
    {
        BasicType: TypePtr;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx): CmObject*;
        Disjunction(ParsingContext* ctx): CmObject*;
        Conjunction(ParsingContext* ctx): CmObject*;
        BitOr(ParsingContext* ctx): CmObject*;
        BitXor(ParsingContext* ctx): CmObject*;
        BitAnd(ParsingContext* ctx): CmObject*;
        Equality(ParsingContext* ctx, var Operator op): CmObject*;
        Relational(ParsingContext* ctx, var Operator op): CmObject*;
        Shift(ParsingContext* ctx, var Operator op): CmObject*;
        Additive(ParsingContext* ctx, var Operator op): CmObject*;
        Multiplicative(ParsingContext* ctx, var Operator op): CmObject*;
        Prefix(ParsingContext* ctx, var Operator op): CmObject*;
        Postfix(ParsingContext* ctx, var UniquePtr<CmObject> expr): CmObject*;
        Primary(ParsingContext* ctx): CmObject*;
        CastExpr(ParsingContext* ctx): CmObject*;
        SizeOfExpr(ParsingContext* ctx): CmObject*;
        ObjectCreationExpr(ParsingContext* ctx): CmObject*;
        NewExpr(ParsingContext* ctx): CmObject*;
        ConstructionExpr(ParsingContext* ctx): CmObject*;
        ArgumentList(ParsingContext* ctx): ExpressionList;
        ExpressionList(ParsingContext* ctx, var ExpressionListHolder exprListHolder): ExpressionList;
    }
    grammar IdentifierGrammar
    {
        Identifier: string;
        QualifiedId: string;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar LiteralGrammar
    {
        Literal: LiteralPtr;
        FloatingLiteral(var double val, var char floatSuffix): Pair<double, char>;
        IntLiteral(var ulong val, var char unsignedSuffix): Pair<ulong, char>;
        HexLiteral(var ulong val, var char unsignedSuffix): Pair<ulong, char>;
        Null;
    }
    grammar NamespaceGrammar
    {
        NamespaceImport: System.Text.Parsing.CmObjectModel.UsingObjectPtr;
        Alias: System.Text.Parsing.CmObjectModel.UsingObjectPtr;
    }
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx): Statement*;
        ControlStatement(ParsingContext* ctx): Statement*;
        ReturnStatement(ParsingContext* ctx): Statement*;
        ConditionalStatement(ParsingContext* ctx): Statement*;
        SwitchStatement(ParsingContext* ctx): SwitchStatementPtr;
        CaseStatement(ParsingContext* ctx): CaseStatementPtr;
        CaseExprList(ParsingContext* ctx): ExpressionList;
        DefaultStatement(ParsingContext* ctx): DefaultStatementPtr;
        GotoCaseStatement(ParsingContext* ctx): Statement*;
        GotoDefaultStatement(ParsingContext* ctx): Statement*;
        WhileStatement(ParsingContext* ctx): Statement*;
        DoStatement(ParsingContext* ctx): Statement*;
        RangeForStatement(ParsingContext* ctx, var UniquePtr<RangeForStatement> holder, var UniquePtr<CmObject> varTypeHolder): RangeForStatementPtr;
        ForStatement(ParsingContext* ctx): ForStatementPtr;
        ForInitStatement(ParsingContext* ctx): Statement*;
        CompoundStatement(ParsingContext* ctx): CompoundStatementPtr;
        BreakStatement(ParsingContext* ctx): Statement*;
        ContinueStatement(ParsingContext* ctx): Statement*;
        GotoStatement(ParsingContext* ctx): Statement*;
        TypedefStatement(ParsingContext* ctx): Statement*;
        SimpleStatement(ParsingContext* ctx, var UniquePtr<CmObject> expr): Statement*;
        AssignmentStatement(ParsingContext* ctx, var UniquePtr<CmObject> targetHolder): Statement*;
        ConstructionStatement(ParsingContext* ctx): Statement*;
        Initialization(ParsingContext* ctx): ExpressionList;
        DeleteStatement(ParsingContext* ctx): Statement*;
        DestroyStatement(ParsingContext* ctx): Statement*;
        ThrowStatement(ParsingContext* ctx): Statement*;
        TryStatement(ParsingContext* ctx): Statement*;
        ExceptionHandlers(ParsingContext* ctx): List<ExceptionHandler*>;
        ExceptionHandler(List<ExceptionHandler*>* handlers, ParsingContext* ctx);
        AssertStatement(ParsingContext* ctx): Statement*;
    }
    grammar TemplateGrammar
    {
        TemplateId(ParsingContext* ctx, var UniquePtr<TemplateId> templateId): TemplateId*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx, var UniquePtr<DerivedTypeExpr> expr): CmObject*;
        PrefixTypeExpr(ParsingContext* ctx, DerivedTypeExpr* expr);
        PostfixTypeExpr(ParsingContext* ctx, DerivedTypeExpr* expr);
        PrimaryTypeExpr(ParsingContext* ctx, DerivedTypeExpr* expr);
    }
}
